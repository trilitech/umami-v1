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
    | InvalidEstimationResults => I18n.Errors.invalid_estimation_results->Some
    | _ => None,
  );

let extractBatchRevealEstimation = (~batchSize, estimations) => {
  switch (estimations->Array.get(0)) {
  | Some((res: ReTaquitoTypes.Estimation.result))
      when estimations->Array.length == batchSize + 1 =>
    Ok((estimations->Array.sliceToEnd(1), Some(res)))
  | Some(_) => Ok((estimations, None))
  | None => Error(InvalidEstimationResults)
  };
};

let handleCustomOptions =
    (results: Toolkit.Estimation.result, (fee, storageLimit, gasLimit)) =>
  Protocol.Simulation.{
    fee:
      fee
      ->Option.getWithDefault(
          results.Toolkit.Estimation.suggestedFeeMutez + results.burnFeeMutez,
        )
      ->Tez.fromMutezInt,
    storageLimit:
      storageLimit->Option.getWithDefault(results.storageLimit) + 100,
    gasLimit: gasLimit->Option.getWithDefault(results.gasLimit) + 100,
  };

let handleReveal = (r: ReTaquitoTypes.Estimation.result) =>
  Protocol.Simulation.{
    fee: r.suggestedFeeMutez->Tez.fromMutezInt,
    storageLimit: r.storageLimit,
    gasLimit: r.gasLimit,
  };

let handleEstimationResults = (results, reveal, options) => {
  let simulations =
    Array.zip(results, options)
    ->Array.map(((est, customValues)) => {
        handleCustomOptions(est, customValues)
      });

  let revealSimulation = reveal->Option.map(handleReveal);

  Protocol.Simulation.{simulations, revealSimulation};
};

module Rpc = {
  let getBalance = (endpoint, ~address, ~params=?, ()) => {
    let%AwaitMap balance =
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

  let getConstants = endpoint =>
    RPCClient.create(endpoint)
    ->RPCClient.getConstants()
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

    let%Await tr = LedgerAPI.init();

    let%Await data =
      LedgerAPI.getMasterKey(~prompt=false, tr)
      ->Promise.map(pkh => pkh->Result.flatMap(keyData));

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
    | CustomAuthSigner(ReCustomAuthSigner.t)
    | Password(string);

  let readSecretKey = (address, signingIntent, dirpath) => {
    let%Await (kind, key) = Wallet.readSecretFromPkh(address, dirpath);

    switch (kind, signingIntent) {
    | (Encrypted, Password(s)) => readEncryptedKey(key, s)
    | (Unencrypted, _) => readUnencryptedKey(key)
    | (Ledger, LedgerCallback(callback)) => readLedgerKey(callback, key)

    | (CustomAuth(_), CustomAuthSigner(s)) =>
      s->ReCustomAuthSigner.toSigner->Promise.ok
    | (CustomAuth(_), _)
    | (Encrypted, _)
    | (Ledger, _) => ReTaquitoError.SignerIntentInconsistency->Promise.err
    };
  };
};

module Delegation = {
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

  let prepareSet = (~source, Protocol.{delegate, fee}: Protocol.delegation) => {
    let feeBignum = fee->Option.map(Tez.toBigNumber);
    Toolkit.prepareDelegate(~source, ~delegate, ~fee=?feeBignum, ());
  };
};

module Origination = {
  open Protocol;
  let prepare = (~source, origination) => {
    let balance = origination.balance->Option.map(Tez.toBigNumber);
    let fee = origination.fee->Option.map(Tez.toBigNumber);

    Toolkit.prepareOriginate(
      ~source,
      ~balance?,
      ~code=origination.code,
      ~storage=origination.storage,
      ~delegate=?origination.delegate,
      ~fee?,
      (),
    );
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

module type Contract = {
  type t;
  let at: (Toolkit.contract, PublicKeyHash.t) => Js.Promise.t(t);
};

module type ContractCache = {
  type t;
  type contract;
  let make: ReTaquito.Toolkit.toolkit => t;
  let findContract: (t, PublicKeyHash.t) => Promise.t(contract);
  let clear: t => unit;
};

module ContractCache =
       (Contract: Contract)
       : (ContractCache with type contract := Contract.t) => {
  type t = {
    contracts: MutableMap.String.t(Promise.t(Contract.t)),
    toolkit: Toolkit.toolkit,
  };

  let make = toolkit => {contracts: MutableMap.String.make(), toolkit};

  let findContract = (cache, token: PublicKeyHash.t) =>
    switch (MutableMap.String.get(cache.contracts, (token :> string))) {
    | Some(c) => c
    | None =>
      let c =
        cache.toolkit.contract
        ->(Contract.at(token))
        ->ReTaquitoError.fromPromiseParsed;
      cache.contracts->MutableMap.String.set((token :> string), c);
      c;
    };

  /* Technically never used for now */
  let clear = cache => cache.contracts->MutableMap.String.clear;
};

module FA12Cache = ContractCache(Contracts.FA12);
module FA2Cache = ContractCache(Contracts.FA2);
module Tzip12Cache = ContractCache(Contracts.Tzip12Tzip16Contract);

module Transfer = {
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

    let%AwaitMap c = contractCache->FA12Cache.findContract(token);
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

    let%AwaitMap c = contractCache->FA2Cache.findContract(token);
    c##methods.Types.FA2.transfer(. transferParams).toTransferParams(.
      sendParams,
    );
  };

  let makeTransferMichelsonParameter = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(a), Some(b)) =>
      Some(ReTaquitoTypes.Transfer.Entrypoint.{entrypoint: a, value: b})
    | _ => None
    };

  let mapFA2Transfers = (txs: list(Transfer.transferFA2)) => {
    txs
    ->List.map(
        (Transfer.{tokenId, content: {amount: {amount}, destination}}) =>
        ReTaquitoTypes.FA2.{
          to_: destination,
          token_id:
            tokenId->Int64.of_int->BigNumber.fromInt64->BigNumber.toFixed,
          amount: amount->TokenRepr.Unit.toBigNumber->BigNumber.toFixed,
        }
      )
    ->List.toArray;
  };

  let prepareTransfer = Toolkit.prepareTransfer;

  let prepare = (fa12Cache, fa2Cache, source: PublicKeyHash.t, t: Transfer.t) => {
    let options = t.options;

    switch (t.data) {
    | Simple({amount: Tez(amount), destination}) =>
      prepareTransfer(
        ~source,
        ~dest=destination,
        ~amount=amount->Tez.toBigNumber,
        ~fee=?options.fee->Option.map(Tez.toBigNumber),
        ~gasLimit=?options.gasLimit,
        ~storageLimit=?options.storageLimit,
        ~parameter=?
          makeTransferMichelsonParameter(
            ~entrypoint=options.entrypoint,
            ~parameter=options.parameter,
          ),
        (),
      )
      ->Ok
      ->Promise.value

    | Simple({amount: Token(tokenAmount), destination}) =>
      switch (tokenAmount.token.kind) {
      | FA1_2 =>
        prepareFA12Transfer(
          fa12Cache,
          ~source,
          ~token=tokenAmount.token.address,
          ~dest=destination,
          ~amount=tokenAmount.amount->TokenRepr.Unit.toBigNumber,
          ~fee=?options.fee->Option.map(Tez.toBigNumber),
          ~gasLimit=?options.gasLimit,
          ~storageLimit=?options.storageLimit,
          (),
        )
      | FA2(tokenId) =>
        let batch =
          mapFA2Transfers([
            {
              tokenId,
              content: {
                destination,
                amount: tokenAmount,
              },
            },
          ]);
        let transferParams = [|
          ReTaquitoTypes.FA2.{from_: source, txs: batch},
        |];
        prepareFA2Transfer(
          fa2Cache,
          ~token=tokenAmount.token.address,
          ~transferParams,
          ~fee=?options.fee->Option.map(Tez.toBigNumber),
          ~gasLimit=?options.gasLimit,
          ~storageLimit=?options.storageLimit,
          (),
        );
      }
    | FA2Batch({address, transfers}) =>
      let batch = mapFA2Transfers(transfers);
      let transferParams = [|
        ReTaquitoTypes.FA2.{from_: source, txs: batch},
      |];
      prepareFA2Transfer(
        fa2Cache,
        ~token=address,
        ~transferParams,
        ~fee=?options.fee->Option.map(Tez.toBigNumber),
        ~gasLimit=?options.gasLimit,
        ~storageLimit=?options.storageLimit,
        (),
      );
    };
  };
};

module Batch = {
  type params =
    | DelegationParams(Toolkit.delegateParams)
    | OriginationParams(Toolkit.originateParams)
    | TransferParams(Toolkit.transferParams);

  let prepareOperations = (op: Protocol.batch, endpoint, source) => {
    let fa12Cache = endpoint->Toolkit.create->FA12Cache.make;
    let fa2Cache = endpoint->Toolkit.create->FA2Cache.make;

    op.managers
    ->Array.map(
        fun
        | Protocol.Origination(o) =>
          Origination.prepare(~source, o)->OriginationParams->Promise.ok
        | Protocol.Delegation(d) =>
          Delegation.prepareSet(~source, d)->DelegationParams->Promise.ok
        | Protocol.Transaction(t) =>
          Transfer.prepare(fa12Cache, fa2Cache, source, t)
          ->Promise.mapOk(v => TransferParams(v)),
      );
  };

  let rewriteKinds = txs =>
    txs->List.map(
      fun
      | TransferParams(tr) =>
        TransferParams({
          ...tr,
          kind: ReTaquitoTypes.Operation.transactionKind,
        })
      | d => d,
    );

  let run = (~endpoint, ~baseDir, ~source, ~ops, ~signingIntent, ()) => {
    let tk = Toolkit.create(endpoint);
    let%Await signer = Signer.readSecretKey(source, signingIntent, baseDir);
    let provider = Toolkit.{signer: signer};
    tk->Toolkit.setProvider(provider);
    let%Await ops =
      ops
      ->prepareOperations(endpoint, source)
      ->List.fromArray
      ->Promise.all
      ->Promise.map(Result.collect);
    let ops = ops->rewriteKinds;
    let batch = tk.contract->Toolkit.Batch.make;
    ops
    ->List.reduce(batch, (b, op) =>
        switch (op) {
        | DelegationParams(d) => b->Toolkit.Batch.withDelegation(d)
        | TransferParams(t) => b->Toolkit.Batch.withTransfer(t)
        | OriginationParams(t) => b->Toolkit.Batch.withOrigination(t)
        }
      )
    ->Toolkit.Batch.send
    ->ReTaquitoError.fromPromiseParsed;
  };

  module Estimate = {
    let patchEstimationWithHardLimits = (~endpoint, ~ops: array(params)) => {
      let%Await {
        hard_gas_limit_per_operation,
        hard_storage_limit_per_operation,
      } =
        Rpc.getConstants(endpoint);

      let hardGasLimit = hard_gas_limit_per_operation->ReBigNumber.toInt;
      let hardStorageLimit =
        hard_storage_limit_per_operation->ReBigNumber.toInt;

      let mapOptions = (gasLimit, storageLimit) => {
        gasLimit->Option.mapDefault(false, g => g >= hardGasLimit)
          ? Error(ReTaquitoError.GasExhaustedAboveLimit)
          : storageLimit->Option.mapDefault(false, g => g >= hardStorageLimit)
              ? Error(ReTaquitoError.StorageExhaustedAboveLimit)
              : (
                  gasLimit == None ? hardGasLimit->Some : gasLimit,
                  storageLimit == None ? hardStorageLimit->Some : storageLimit,
                )
                ->Ok;
      };

      // Only use the hard limit if not provided by the user and checks if one
      // of the user's limit is above the maximum
      ops
      ->Array.map(
          fun
          | TransferParams(tp) =>
            mapOptions(tp.gasLimit, tp.storageLimit)
            ->Result.map(((gasLimit, storageLimit)) =>
                TransferParams({...tp, gasLimit, storageLimit})
              )

          | OriginationParams(op) =>
            mapOptions(op.gasLimit, op.storageLimit)
            ->Result.map(((gasLimit, storageLimit)) =>
                OriginationParams({...op, gasLimit, storageLimit})
              )
          | dp => Ok(dp),
        )
      ->Result.collectArray
      ->FutureBase.value;
    };

    let inject = (~endpoint, ~publicKey, ~source, ~ops) => {
      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let%Await ops =
        ops
        ->prepareOperations(endpoint, source)
        ->List.fromArray
        ->Promise.all
        ->Promise.map(Result.collect);

      let ops = ops->rewriteKinds->List.toArray;

      let inject = ops => {
        let ops =
          ops->Array.map(
            fun
            | TransferParams(tp) => Toolkit.Estimation.fromTransferParams(tp)
            | DelegationParams(dp) =>
              Toolkit.Estimation.fromDelegateParams(dp)
            | OriginationParams(op) =>
              Toolkit.Estimation.fromOriginationParams(op),
          );

        tk.estimate
        ->Toolkit.Estimation.batch(ops)
        ->ReTaquitoError.fromPromiseParsed;
      };

      let%Ft res = inject(ops);

      switch (res) {
      | Ok(results) => Promise.ok(results)
      | Error(ReTaquitoError.GasExhausted | ReTaquitoError.StorageExhausted) =>
        patchEstimationWithHardLimits(~endpoint, ~ops)
        ->Promise.flatMapOk(inject)
      | Error(e) => Promise.err(e)
      };
    };

    let run =
        (
          ~endpoint,
          ~baseDir,
          ~source: PublicKeyHash.t,
          ~customValues,
          ~ops,
          (),
        ) => {
      let%Await alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%Await publicKey = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let%Await res = inject(~endpoint, ~publicKey, ~source, ~ops);

      let%AwaitMap (simulations, reveal) =
        extractBatchRevealEstimation(
          ~batchSize=ops.managers->Array.length,
          res,
        )
        ->Promise.value;

      handleEstimationResults(simulations, reveal, customValues);
    };
  };
};

module Signature = {
  let signPayload = (~baseDir, ~source, ~signingIntent, ~payload) => {
    let%Await signer = Signer.readSecretKey(source, signingIntent, baseDir);

    signer->ReTaquitoSigner.sign(payload);
  };
};
