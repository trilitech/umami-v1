/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReTaquito;
open ReTaquitoSigner;
open Let;

module Contracts = ReTaquitoContracts;

let revealFee = (~endpoint, source) => {
  let client = RPCClient.create(endpoint);

  let%FResMap k =
    RPCClient.getManagerKey(client, source)->ReTaquitoError.fromPromiseParsed;

  Js.Nullable.isNullable(k) ? default_fee_reveal : 0;
};

let addRevealFee = (~source, ~endpoint, r) => {
  let%FResMap fee = revealFee(~endpoint, source);
  Toolkit.Estimation.{...r, totalCost: fee + r.totalCost, revealFee: fee};
};

let handleCustomOptions =
    (results: Toolkit.Estimation.result, (fee, storageLimit, gasLimit)) => {
  ...results,
  Toolkit.Estimation.customFeeMutez:
    fee->Option.getWithDefault(
      results.Toolkit.Estimation.suggestedFeeMutez + results.burnFeeMutez,
    ),
  storageLimit: storageLimit->Option.getWithDefault(results.storageLimit),
  gasLimit: gasLimit->Option.getWithDefault(results.gasLimit),
};
let handleEstimationResults = ((results, revealFee), options, index) => {
  switch (index) {
  | Some(index) =>
    let customOptions =
      options[index]->Option.getWithDefault((None, None, None));
    let%Res res =
      results
      ->Array.get(index)
      ->ResultEx.fromOption(Errors.Generic("No transfer with such index"));
    res->handleCustomOptions(customOptions)->Ok;
  | None =>
    open Toolkit.Estimation;
    let init = {
      totalCost: 0,
      gasLimit: 0,
      storageLimit: 0,
      revealFee: 0,
      minimalFeeMutez: 0,
      suggestedFeeMutez: 0,
      burnFeeMutez: 0,
      customFeeMutez: 0,
    };

    /* Same behavior as tezos-client, adding 100 milligas as fees just in case */
    /* also adds default revealFees */
    let {gasLimit, storageLimit, totalCost, customFeeMutez} as est =
      results
      ->Array.zip(options)
      ->Array.reduce(
          init,
          (acc, (est, customValues)) => {
            let est = handleCustomOptions(est, customValues);
            {
              ...est,
              totalCost: acc.totalCost + est.totalCost,
              storageLimit: acc.storageLimit + est.storageLimit,
              gasLimit: acc.gasLimit + est.gasLimit,
              minimalFeeMutez: acc.minimalFeeMutez + est.minimalFeeMutez,
              suggestedFeeMutez: acc.suggestedFeeMutez + est.suggestedFeeMutez,
              burnFeeMutez: acc.burnFeeMutez + est.burnFeeMutez,
              customFeeMutez: acc.customFeeMutez + est.customFeeMutez,
            };
          },
        );

    {
      ...est,
      gasLimit: gasLimit + 100,
      totalCost: totalCost + revealFee,
      customFeeMutez,
      storageLimit: storageLimit + 100,
      revealFee,
    }
    ->Ok;
  };
};

module Balance = {
  let get = (endpoint, ~address, ~params=?, ()) => {
    let%FResMap balance =
      RPCClient.create(endpoint)
      ->RPCClient.getBalance(address, ~params?, ())
      ->ReTaquitoError.fromPromiseParsed;
    balance->BigNumber.toInt64->Tez.ofInt64;
  };
};

module Signer = {
  let readEncryptedKey = (key, passphrase) =>
    MemorySigner.create(
      ~secretKey=key->Js.String2.substringToEnd(~from=10),
      ~passphrase,
      (),
    );

  let readUnencryptedKey = key =>
    MemorySigner.create(
      ~secretKey=key->Js.String2.substringToEnd(~from=12),
      ~passphrase="",
      (),
    );

  let readLedgerKey = (callback, key) => {
    let keyData = ledgerBasePkh =>
      key->Wallet.Ledger.Decode.fromSecretKey(~ledgerBasePkh);

    let%FRes tr = LedgerAPI.init();

    let%FRes data =
      LedgerAPI.getMasterKey(~prompt=false, tr)
      ->Future.map(pkh => pkh->Result.flatMap(keyData));

    callback();
    LedgerAPI.Signer.create(
      tr,
      data.path->DerivationPath.fromTezosBip44,
      data.scheme,
      ~prompt=false,
    );
  };

  type intent =
    | LedgerCallback(unit => unit)
    | Password(string);

  let readSecretKey = (address, signingIntent, dirpath) => {
    let%FRes (kind, key) = Wallet.readSecretFromPkh(address, dirpath);

    switch (kind, signingIntent) {
    | (Encrypted, Password(s)) => readEncryptedKey(key, s)
    | (Unencrypted, _) => readUnencryptedKey(key)
    | (Ledger, LedgerCallback(callback)) => readLedgerKey(callback, key)
    | _ => ReTaquitoError.SignerIntentInconsistency->FutureEx.err
    };
  };
};

module Delegate = {
  exception RejectError(string);

  let get = (endpoint, address: PublicKeyHash.t) => {
    let tk = Toolkit.create(endpoint);

    Toolkit.getDelegate(tk.tz, address)
    |> Js.Promise.then_(v => Js.Promise.resolve(Js.Nullable.toOption(v)))
    |> Js.Promise.catch(e =>
         if (Obj.magic(e)##status == 404) {
           Js.Promise.resolve(None);
         } else {
           Js.Promise.reject(RejectError(e->Js.String.make));
         }
       )
    |> ReTaquitoError.fromPromiseParsed;
  };

  let set =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~delegate: option(PublicKeyHash.t),
        ~signingIntent: Signer.intent,
        ~fee=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let fee = fee->Option.map(v => v->Tez.toInt64->BigNumber.fromInt64);
    let%FRes signer = Signer.readSecretKey(source, signingIntent, baseDir);
    let provider = Toolkit.{signer: signer};
    tk->Toolkit.setProvider(provider);
    let dg = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());
    tk.contract->Toolkit.setDelegate(dg)->ReTaquitoError.fromPromiseParsed;
  };

  module Estimate = {
    let set =
        (
          ~endpoint,
          ~baseDir,
          ~source: PublicKeyHash.t,
          ~delegate=?,
          ~fee=?,
          (),
        ) => {
      let%FRes alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%FRes pk = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let feeBignum = fee->Option.map(Tez.toBigNumber);
      let sd =
        Toolkit.prepareDelegate(~source, ~delegate, ~fee=?feeBignum, ());
      Js.log(sd);

      let%FRes res =
        tk.estimate
        ->Toolkit.Estimation.setDelegate(sd)
        ->ReTaquitoError.fromPromiseParsed;

      res
      ->handleCustomOptions((
          fee->Option.map(Tez.unsafeToMutezInt),
          None,
          None,
        ))
      ->addRevealFee(~source, ~endpoint);
    };
  };
};

module Operations = {
  let confirmation = (endpoint, ~hash, ~blocks=?, ()) => {
    let tk = Toolkit.create(endpoint);
    let res =
      tk.operation->Toolkit.Operation.create(hash)
      |> Js.Promise.then_(op => op->Toolkit.Operation.confirmation(~blocks?));
    res->ReTaquitoError.fromPromiseParsed;
  };
};

module Transfer = {
  module ContractCache = {
    type t = {
      contracts:
        MutableMap.String.t(Js.Promise.t(Contracts.FA12.Abstraction.t)),
      toolkit: Toolkit.toolkit,
    };

    let make = toolkit => {contracts: MutableMap.String.make(), toolkit};

    let findContract = (cache, token: PublicKeyHash.t) =>
      switch (MutableMap.String.get(cache.contracts, (token :> string))) {
      | Some(c) => c
      | None =>
        let c = cache.toolkit.contract->(ReTaquitoContracts.FA12.at(token));
        cache.contracts->MutableMap.String.set((token :> string), c);
        c;
      };

    /* Technically never used for now */
    let _clear = cache => cache.contracts->MutableMap.String.clear;
  };

  let prepareFA12Transfer =
      (
        contractCache,
        ~source: PublicKeyHash.t,
        ~token: PublicKeyHash.t,
        ~dest,
        ~amount,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    let sendParams =
      Toolkit.makeSendParams(
        ~amount=BigNumber.fromInt64(0L),
        ~fee?,
        ~gasLimit?,
        ~storageLimit?,
        (),
      );

    let%FResMap c =
      contractCache
      ->ContractCache.findContract(token)
      ->ReTaquitoError.fromPromiseParsed;
    c
    ->Contracts.FA12.transfer(source, dest, amount->BigNumber.toFixed)
    ->Contracts.FA12.toTransferParams(sendParams);
  };

  let prepareTransfer = Toolkit.prepareTransfer;

  let makeTransferMichelsonParameter = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(a), Some(b)) =>
      Some({ProtocolOptions.TransactionParameters.entrypoint: a, value: b})
    | _ => None
    };

  let prepareTransfers = (txs, endpoint, source: PublicKeyHash.t) => {
    let contractCache = endpoint->Toolkit.create->ContractCache.make;
    txs
    ->List.map((tx: Transfer.elt) =>
        switch (tx.amount) {
        | Tez(amount) =>
          prepareTransfer(
            ~source,
            ~dest=tx.destination,
            ~amount=amount->Tez.toBigNumber,
            ~fee=?tx.tx_options.fee->Option.map(Tez.toBigNumber),
            ~gasLimit=?tx.tx_options.gasLimit,
            ~storageLimit=?tx.tx_options.storageLimit,
            ~parameter=?
              makeTransferMichelsonParameter(
                ~entrypoint=tx.tx_options.entrypoint,
                ~parameter=tx.tx_options.parameter,
              ),
            (),
          )
          ->Ok
          ->Future.value
        | Token(amount, token) =>
          prepareFA12Transfer(
            contractCache,
            ~source,
            ~token=token.TokenRepr.address,
            ~dest=tx.destination,
            ~amount=amount->TokenRepr.Unit.toBigNumber,
            ~fee=?tx.tx_options.fee->Option.map(Tez.toBigNumber),
            ~gasLimit=?tx.tx_options.gasLimit,
            ~storageLimit=?tx.tx_options.storageLimit,
            (),
          )
        }
      )
    ->Future.all;
  };

  let batch =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~transfers:
           (ReTaquito.endpoint, PublicKeyHash.t) =>
           Future.t(list(Belt.Result.t(Toolkit.transferParams, Errors.t))),
        ~signingIntent,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let%FRes signer = Signer.readSecretKey(source, signingIntent, baseDir);
    let provider = Toolkit.{signer: signer};
    tk->Toolkit.setProvider(provider);
    let%FRes txs = endpoint->transfers(source)->Future.map(ResultEx.collect);
    let txs = txs->List.map(tr => {...tr, kind: opKindTransaction});
    let batch = tk.contract->Toolkit.Batch.make;
    txs
    ->List.reduce(batch, Toolkit.Batch.withTransfer)
    ->Toolkit.Batch.send
    ->ReTaquitoError.fromPromiseParsed;
  };

  module Estimate = {
    let batch =
        (
          ~endpoint,
          ~baseDir,
          ~source: PublicKeyHash.t,
          ~transfers:
             (ReTaquito.endpoint, PublicKeyHash.t) =>
             Future.t(list(Belt.Result.t(Toolkit.transferParams, Errors.t))),
          (),
        ) => {
      let%FRes alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%FRes pk = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let%FRes txs =
        endpoint->transfers(source)->Future.map(ResultEx.collect);

      let%FRes res =
        tk.estimate
        ->Toolkit.Estimation.batch(
            txs
            ->List.map(tr => {...tr, kind: opKindTransaction})
            ->List.toArray,
          )
        ->ReTaquitoError.fromPromiseParsed;

      let%FResMap revealFee = revealFee(~endpoint, source);
      (res, revealFee);
    };
  };
};

module Signature = {
  let signPayload = (~baseDir, ~source, ~signingIntent, ~payload) => {
    let%FRes signer = Signer.readSecretKey(source, signingIntent, baseDir);

    signer->ReTaquitoSigner.sign(payload);
  };
};
