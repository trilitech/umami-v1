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

type Errors.t +=
  | InvalidEstimationResults;

let () =
  Errors.registerHandler(
    "Taquito",
    fun
    | InvalidEstimationResults => I18n.errors#invalid_estimation_results->Some
    | _ => None,
  );

let extractBatchRevealFee = (xs, estimations) => {
  switch (estimations->Array.get(0)) {
  | Some((res: ReTaquitoTypes.Estimation.result))
      when estimations->Array.length == xs->Array.length + 1 =>
    Ok((estimations->Array.sliceToEnd(1), res.suggestedFeeMutez))
  | Some(_) => Ok((estimations, 0))
  | None => Error(InvalidEstimationResults)
  };
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
      Array.zip(results, options)
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

module Rpc = {
  let getBalance = (endpoint, ~address, ~params=?, ()) => {
    let%FResMap balance =
      RPCClient.create(endpoint)
      ->RPCClient.getBalance(address, ~params?, ())
      ->ReTaquitoError.fromPromiseParsed;
    balance->BigNumber.toInt64->Tez.ofInt64;
  };

  let getChainId = endpoint =>
    RPCClient.create(endpoint)
    ->RPCClient.getChainId()
    ->ReTaquitoError.fromPromiseParsed;

  let getBlockHeader = endpoint =>
    RPCClient.create(endpoint)
    ->RPCClient.getBlockHeader()
    ->ReTaquitoError.fromPromiseParsed;
};

module Signer = {
  let readEncryptedKey = (key, passphrase) =>
    MemorySigner.create(~secretKey=key, ~passphrase, ());

  let readUnencryptedKey = key =>
    MemorySigner.create(~secretKey=key, ~passphrase="", ());

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

      let%FRes res =
        tk.estimate
        ->Toolkit.Estimation.batchDelegation([|sd|])
        ->ReTaquitoError.fromPromiseParsed;

      let%FRes (res, revealFee) =
        extractBatchRevealFee([|sd|], res)->Future.value;

      let%FResMap res =
        switch (res) {
        | [|res|] => FutureEx.ok(res)
        | _ => FutureEx.err(InvalidEstimationResults)
        };

      let res =
        res->handleCustomOptions((
          fee->Option.map(Tez.unsafeToMutezInt),
          None,
          None,
        ));

      Toolkit.Estimation.{
        ...res,
        totalCost: revealFee + res.totalCost,
        revealFee,
      };
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
  module type Contract = {
    type t;
    let at: (Toolkit.contract, PublicKeyHash.t) => Js.Promise.t(t);
  };

  module ContractCache = (Contract: Contract) => {
    type t = {
      contracts: MutableMap.String.t(Js.Promise.t(Contract.t)),
      toolkit: Toolkit.toolkit,
    };

    let make = toolkit => {contracts: MutableMap.String.make(), toolkit};

    let findContract = (cache, token: PublicKeyHash.t) =>
      switch (MutableMap.String.get(cache.contracts, (token :> string))) {
      | Some(c) => c
      | None =>
        let c = cache.toolkit.contract->(Contract.at(token));
        cache.contracts->MutableMap.String.set((token :> string), c);
        c;
      };

    /* Technically never used for now */
    let _clear = cache => cache.contracts->MutableMap.String.clear;
  };

  module FA12Cache = ContractCache(Contracts.FA12);
  module FA2Cache = ContractCache(Contracts.FA2);

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
      ->FA12Cache.findContract(token)
      ->ReTaquitoError.fromPromiseParsed;
    let transfer =
      c->Contracts.FA12.transfer(source, dest, amount->BigNumber.toFixed);
    transfer.toTransferParams(. sendParams);
  };

  let prepareFA2Transfer =
      (
        contractCache,
        ~token,
        ~transferParams,
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
      ->FA2Cache.findContract(token)
      ->ReTaquitoError.fromPromiseParsed;
    c##methods.Types.FA2.transfer(. transferParams).toTransferParams(.
      sendParams,
    );
  };

  let prepareTransfer = Toolkit.prepareTransfer;

  let makeTransferMichelsonParameter = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(a), Some(b)) =>
      Some({ProtocolOptions.TransactionParameters.entrypoint: a, value: b})
    | _ => None
    };

  let consolidateTransferOptions =
      (
        batch: ProtocolOptions.transferOptions,
        tx: ProtocolOptions.transferOptions,
      ) =>
    ProtocolOptions.{
      fee: OptionEx.mapOrKeep(batch.fee, tx.fee, max),
      gasLimit: OptionEx.mapOrKeep(batch.gasLimit, tx.gasLimit, (+)),
      storageLimit:
        OptionEx.mapOrKeep(batch.storageLimit, tx.storageLimit, (+)),
      // parameter and entrypoint are irrelevant for tokens, which are already
      // translated as a parameter and an entrypoint
      parameter: None,
      entrypoint: None,
    };

  /* FA2 implements batchs directly into the transfer entrypoint. As such, we
     can take multiple consecutive transactions into the same contract and generate
     the respective FA2 batch */
  let rec consolidateFA2Transfers =
          (contractAddress, batch, options, txs)
          : (
              array(Types.FA2.transaction),
              ProtocolOptions.transferOptions,
              list(Transfer.elt),
            ) => {
    switch (txs) {
    | [Transfer.{amount: Tez(_)}, ..._]
    | [{amount: Token(_, {kind: FA1_2})}, ..._]
    | [] => (batch, options, txs)

    | [{amount: Token(_, token)}, ..._]
        when token.address != contractAddress => (
        batch,
        options,
        txs,
      )

    | [
        {amount: Token(amount, {kind: FA2(id)}), destination, tx_options},
        ...txs,
      ] =>
      consolidateFA2Transfers(
        contractAddress,
        Array.concat(
          batch,
          [|
            {
              to_: destination,
              token_id:
                id->Int64.of_int->BigNumber.fromInt64->BigNumber.toFixed,
              amount: amount->TokenRepr.Unit.toBigNumber->BigNumber.toFixed,
            },
          |],
        ),
        consolidateTransferOptions(options, tx_options),
        txs,
      )
    };
  };

  let rec prepareTransfers =
          (fa12Cache, fa2Cache, source: PublicKeyHash.t, txs, prepared) => {
    switch (txs) {
    | [] => prepared->List.reverse
    | [Transfer.{amount: Tez(amount), destination, tx_options}, ...txs] =>
      let tx =
        prepareTransfer(
          ~source,
          ~dest=destination,
          ~amount=amount->Tez.toBigNumber,
          ~fee=?tx_options.fee->Option.map(Tez.toBigNumber),
          ~gasLimit=?tx_options.gasLimit,
          ~storageLimit=?tx_options.storageLimit,
          ~parameter=?
            makeTransferMichelsonParameter(
              ~entrypoint=tx_options.entrypoint,
              ~parameter=tx_options.parameter,
            ),
          (),
        )
        ->Ok
        ->Future.value;
      prepareTransfers(fa12Cache, fa2Cache, source, txs, [tx, ...prepared]);

    | [
        {
          amount: Token(amount, {kind: FA1_2, address}),
          destination,
          tx_options,
        },
        ...txs,
      ] =>
      let tx =
        prepareFA12Transfer(
          fa12Cache,
          ~source,
          ~token=address,
          ~dest=destination,
          ~amount=amount->TokenRepr.Unit.toBigNumber,
          ~fee=?tx_options.fee->Option.map(Tez.toBigNumber),
          ~gasLimit=?tx_options.gasLimit,
          ~storageLimit=?tx_options.storageLimit,
          (),
        );
      prepareTransfers(fa12Cache, fa2Cache, source, txs, [tx, ...prepared]);
    | [{amount: Token(_, {kind: FA2(_), address})}, ..._] =>
      let (batch, options, txs) =
        consolidateFA2Transfers(
          address,
          [||],
          ProtocolOptions.emptyTransferOptions,
          txs,
        );
      let transferParams = [|
        ReTaquitoTypes.FA2.{from_: source, txs: batch},
      |];
      let tx =
        prepareFA2Transfer(
          fa2Cache,
          ~token=address,
          ~transferParams,
          ~fee=?options.fee->Option.map(Tez.toBigNumber),
          ~gasLimit=?options.gasLimit,
          ~storageLimit=?options.storageLimit,
          (),
        );
      prepareTransfers(fa12Cache, fa2Cache, source, txs, [tx, ...prepared]);
    };
  };

  let prepareTransfers = (txs, endpoint, source) => {
    let fa12Cache = endpoint->Toolkit.create->FA12Cache.make;
    let fa2Cache = endpoint->Toolkit.create->FA2Cache.make;
    prepareTransfers(fa12Cache, fa2Cache, source, txs, [])->Future.all;
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

      let txs =
        txs->List.map(tr => {...tr, kind: opKindTransaction})->List.toArray;

      let%FRes res =
        tk.estimate
        ->Toolkit.Estimation.batch(txs)
        ->ReTaquitoError.fromPromiseParsed;

      extractBatchRevealFee(txs, res)->Future.value;
    };
  };
};

module Signature = {
  let signPayload = (~baseDir, ~source, ~signingIntent, ~payload) => {
    let%FRes signer = Signer.readSecretKey(source, signingIntent, baseDir);

    signer->ReTaquitoSigner.sign(payload);
  };
};
