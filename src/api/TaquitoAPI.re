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

let convertLedgerError = res =>
  res->ResultEx.mapError(e => e->Wallet.convertLedgerError);

let convertWalletError = res =>
  res->ResultEx.mapError(e => ErrorHandler.Wallet(e));

let convertToErrorHandler = res =>
  ReTaquitoError.fromPromiseParsedWrapper(ErrorHandler.taquito, res);

let revealFee = (~endpoint, source) => {
  let client = RPCClient.create(endpoint);

  RPCClient.getManagerKey(client, source)
  ->ReTaquitoError.fromPromiseParsed
  ->Future.mapOk(k => Js.Nullable.isNullable(k) ? default_fee_reveal : 0)
  ->Future.mapError(ErrorHandler.taquito);
};

let addRevealFee = (~source, ~endpoint, r) => {
  revealFee(~endpoint, source)
  ->Future.mapOk(fee =>
      Toolkit.Estimation.{...r, totalCost: fee + r.totalCost, revealFee: fee}
    );
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
    Js.log(customOptions);
    results
    ->Array.get(index)
    ->FutureEx.fromOption(
        ~error=
          ErrorHandler.taquito(
            ReTaquitoError.Generic("No transfer with such index"),
          ),
      )
    ->Future.mapOk(res => res->handleCustomOptions(customOptions));
  | None =>
    results
    ->Array.zip(options)
    ->Array.reduce(
        Toolkit.Estimation.{
          totalCost: 0,
          gasLimit: 0,
          storageLimit: 0,
          revealFee: 0,
          minimalFeeMutez: 0,
          suggestedFeeMutez: 0,
          burnFeeMutez: 0,
          customFeeMutez: 0,
        },
        (
          {
            totalCost,
            gasLimit,
            storageLimit,
            minimalFeeMutez,
            suggestedFeeMutez,
            burnFeeMutez,
            customFeeMutez,
          },
          (est, customValues),
        ) => {
          let est = handleCustomOptions(est, customValues);
          {
            ...est,
            totalCost: totalCost + est.totalCost,
            storageLimit: storageLimit + est.storageLimit,
            gasLimit: gasLimit + est.gasLimit,
            minimalFeeMutez: minimalFeeMutez + est.minimalFeeMutez,
            suggestedFeeMutez: suggestedFeeMutez + est.suggestedFeeMutez,
            burnFeeMutez: burnFeeMutez + est.burnFeeMutez,
            customFeeMutez: customFeeMutez + est.customFeeMutez,
          };
        },
      )
    ->(
        (
          Toolkit.Estimation.{
            gasLimit,
            storageLimit,
            totalCost,
            customFeeMutez,
          } as est,
        ) =>
          Toolkit.Estimation.{
            ...est,
            gasLimit: gasLimit + 100,
            totalCost: totalCost + revealFee,
            customFeeMutez,
            storageLimit: storageLimit + 100,
            revealFee,
          }
      )
    ->Ok
    ->Future.value
  };
};

module Balance = {
  let get = (endpoint, ~address, ~params=?, ()) => {
    RPCClient.create(endpoint)
    ->RPCClient.getBalance(address, ~params?, ())
    ->convertToErrorHandler
    ->Future.mapOk(v => v->BigNumber.toInt64->Tez.ofInt64);
  };
};

module Signer = {
  let readEncryptedKey = (key, passphrase) =>
    MemorySigner.create(
      ~secretKey=key->Js.String2.substringToEnd(~from=10),
      ~passphrase,
      (),
    )
    ->Future.mapError(ErrorHandler.taquito);

  let readUnencryptedKey = key =>
    MemorySigner.create(
      ~secretKey=key->Js.String2.substringToEnd(~from=12),
      ~passphrase="",
      (),
    )
    ->Future.mapError(ErrorHandler.taquito);

  let readLedgerKey = key => {
    let keyData = ledgerBasePkh =>
      key
      ->Wallet.Ledger.Decode.fromSecretKey(~ledgerBasePkh)
      ->convertLedgerError
      ->convertWalletError;

    LedgerAPI.init()
    ->Future.flatMapOk(tr =>
        LedgerAPI.getMasterKey(~prompt=false, tr)
        ->Future.map(pkh => pkh->Result.flatMap(keyData))
        ->Future.mapOk(data => (tr, data))
      )
    ->Future.flatMapOk(((tr, data)) =>
        LedgerAPI.Signer.create(
          tr,
          data.path->DerivationPath.fromTezosBip44,
          data.scheme,
          ~prompt=false,
        )
      );
  };

  let readSecretKey = (address, passphrase, dirpath) => {
    Wallet.readSecretFromPkh(address, dirpath)
    ->Future.map(convertWalletError)
    ->Future.flatMapOk(((kind, key)) =>
        switch (kind) {
        | Encrypted => readEncryptedKey(key, passphrase)
        | Unencrypted => readUnencryptedKey(key)
        | Ledger => readLedgerKey(key)
        }
      );
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
    |> (v => convertToErrorHandler(v));
  };

  let set =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~delegate: option(PublicKeyHash.t),
        ~password,
        ~fee=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let fee = fee->Option.map(v => v->Tez.toInt64->BigNumber.fromInt64);

    Signer.readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let dg = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());

        tk.contract->Toolkit.setDelegate(dg)->convertToErrorHandler;
      });
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
        ) =>
      Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
      ->Future.flatMapOk(alias =>
          Wallet.pkFromAlias(~dirpath=baseDir, ~alias, ())
        )
      ->Future.map(convertWalletError)
      ->Future.flatMapOk(pk => {
          let tk = Toolkit.create(endpoint);
          let signer =
            EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
          let provider = Toolkit.{signer: signer};
          tk->Toolkit.setProvider(provider);

          let fee = fee->Option.map(Tez.toBigNumber);
          let sd = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());
          Js.log(sd);

          tk.estimate
          ->Toolkit.Estimation.setDelegate(sd)
          ->convertToErrorHandler;
        })
      ->Future.mapOk(res =>
          res->handleCustomOptions((
            fee->Option.map(Tez.unsafeToMutezInt),
            None,
            None,
          ))
        )
      ->Future.flatMapOk(addRevealFee(~source, ~endpoint));
  };
};

module Operations = {
  let confirmation = (endpoint, ~hash, ~blocks=?, ()) => {
    let tk = Toolkit.create(endpoint);
    let res =
      tk.operation->Toolkit.Operation.create(hash)
      |> Js.Promise.then_(op => op->Toolkit.Operation.confirmation(~blocks?));
    res->convertToErrorHandler;
  };
};

module Transfer = {
  module ContractCache = {
    type t = {
      contracts:
        MutableMap.String.t(Js.Promise.t(Toolkit.FA12.contractAbstraction)),
      toolkit: Toolkit.toolkit,
    };

    let make = toolkit => {contracts: MutableMap.String.make(), toolkit};

    let findContract = (cache, token: PublicKeyHash.t) =>
      switch (MutableMap.String.get(cache.contracts, (token :> string))) {
      | Some(c) => c
      | None =>
        let c = cache.toolkit.contract->Toolkit.FA12.at(token);
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

    contractCache
    ->ContractCache.findContract(token)
    ->convertToErrorHandler
    ->Future.mapOk(c =>
        c.methods
        ->Toolkit.FA12.transfer(source, dest, amount->BigNumber.toFixed)
        ->Toolkit.FA12.toTransferParams(sendParams)
      );
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
          ->Future.tapOk(Js.log)
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
           Future.t(
             list(Belt.Result.t(Toolkit.transferParams, ErrorHandler.t)),
           ),
        ~password,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);

    Signer.readSecretKey(source, password, baseDir)
    ->Future.mapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);
      })
    ->Future.flatMapOk(() =>
        endpoint
        ->transfers(source)
        ->Future.map(ResultEx.collect)
        ->Future.mapOk(txs => {
            txs->List.map(tr => {...tr, kind: opKindTransaction})
          })
      )
    ->Future.flatMapOk(txs => {
        let batch = tk.contract->Toolkit.Batch.make;
        txs
        ->List.reduce(batch, Toolkit.Batch.withTransfer)
        ->Toolkit.Batch.send
        ->convertToErrorHandler;
      });
  };
  module Estimate = {
    let batch =
        (
          ~endpoint,
          ~baseDir,
          ~source: PublicKeyHash.t,
          ~transfers:
             (ReTaquito.endpoint, PublicKeyHash.t) =>
             Future.t(
               list(Belt.Result.t(Toolkit.transferParams, ErrorHandler.t)),
             ),
          (),
        ) => {
      Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
      ->Future.flatMapOk(alias =>
          Wallet.pkFromAlias(~dirpath=baseDir, ~alias, ())
        )
      ->Future.map(convertWalletError)
      ->Future.mapOk(pk => {
          let tk = Toolkit.create(endpoint);
          let signer =
            EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
          let provider = Toolkit.{signer: signer};
          tk->Toolkit.setProvider(provider);
          tk;
        })
      ->Future.flatMapOk(tk =>
          endpoint
          ->transfers(source)
          ->Future.map(ResultEx.collect)
          ->Future.flatMapOk(txs =>
              tk.estimate
              ->Toolkit.Estimation.batch(
                  txs
                  ->List.map(tr => {...tr, kind: opKindTransaction})
                  ->List.toArray,
                )
              ->convertToErrorHandler
            )
        )
      ->Future.flatMapOk(r =>
          revealFee(~endpoint, source)
          ->Future.mapOk(revealFee => (r, revealFee))
        );
    };
  };
};

module Signature = {
  let signPayload = (~baseDir, ~source, ~password, ~payload) => {
    Signer.readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer =>
        signer
        ->ReTaquitoSigner.sign(payload)
        ->Future.mapError(ErrorHandler.taquito)
      );
  };
};
