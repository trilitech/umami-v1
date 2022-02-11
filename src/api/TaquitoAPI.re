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

let extractBatchRevealEstimation = (xs, estimations) => {
  switch (estimations->Array.get(0)) {
  | Some((res: ReTaquitoTypes.Estimation.result))
      when estimations->Array.length == xs->Array.length + 1 =>
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
    let%Await signer = Signer.readSecretKey(source, signingIntent, baseDir);
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
      let%Await alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%Await pk = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let feeBignum = fee->Option.map(Tez.toBigNumber);
      let sd =
        Toolkit.prepareDelegate(~source, ~delegate, ~fee=?feeBignum, ());

      let%Await res =
        tk.estimate
        ->Toolkit.Estimation.batchDelegation([|sd|])
        ->ReTaquitoError.fromPromiseParsed;

      let%Await (res, reveal) =
        extractBatchRevealEstimation([|sd|], res)->Promise.value;

      let%AwaitMap res =
        switch (res) {
        | [|res|] => Promise.ok(res)
        | _ => Promise.err(InvalidEstimationResults)
        };

      let simulation =
        res->handleCustomOptions((
          fee->Option.map(Tez.unsafeToMutezInt),
          None,
          None,
        ));

      Protocol.Simulation.{
        simulations: [|simulation|],
        revealSimulation: reveal->Option.map(handleReveal),
      };
    };
  };
};

module Originate = {
  let originate =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~balance=?,
        ~code,
        ~storage,
        ~delegate=?,
        ~signingIntent: Signer.intent,
        ~fee=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let balance =
      balance->Option.map(v => v->Tez.toInt64->BigNumber.fromInt64);
    let fee = fee->Option.map(v => v->Tez.toInt64->BigNumber.fromInt64);
    let%Await signer = Signer.readSecretKey(source, signingIntent, baseDir);
    let provider = Toolkit.{signer: signer};
    tk->Toolkit.setProvider(provider);
    let og =
      Toolkit.prepareOriginate(
        ~source,
        ~balance?,
        ~code,
        ~storage,
        ~delegate?,
        ~fee?,
        (),
      );
    tk.contract->Toolkit.originate(og)->ReTaquitoError.fromPromiseParsed;
  };

  module Estimate = {
    let originate =
        (
          ~endpoint,
          ~baseDir,
          ~source: PublicKeyHash.t,
          ~balance=?,
          ~code,
          ~storage,
          ~delegate=?,
          ~fee=?,
          (),
        ) => {
      let%Await alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%Await pk = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let balanceBignum = balance->Option.map(Tez.toBigNumber);
      let feeBignum = fee->Option.map(Tez.toBigNumber);
      let so =
        Toolkit.prepareOriginate(
          ~source,
          ~balance=?balanceBignum,
          ~code,
          ~storage,
          ~delegate?,
          ~fee=?feeBignum,
          (),
        );

      let%Await res =
        tk.estimate
        ->Toolkit.Estimation.batchOrigination([|so|])
        ->ReTaquitoError.fromPromiseParsed;

      let%Await (res, reveal) =
        extractBatchRevealEstimation([|so|], res)->Promise.value;

      let%AwaitMap res =
        switch (res) {
        | [|res|] => Promise.ok(res)
        | _ => Promise.err(InvalidEstimationResults)
        };

      let simulation =
        res->handleCustomOptions((
          fee->Option.map(Tez.unsafeToMutezInt),
          None,
          None,
        ));

      Protocol.Simulation.{
        simulations: [|simulation|],
        revealSimulation: reveal->Option.map(handleReveal),
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

  let prepareTransfer = Toolkit.prepareTransfer;

  let makeTransferMichelsonParameter = (~entrypoint, ~parameter) =>
    switch (entrypoint, parameter) {
    | (Some(a), Some(b)) =>
      Some(ReTaquitoTypes.Transfer.Parameters.{entrypoint: a, value: b})
    | _ => None
    };

  let consolidateTransferOptions =
      (
        batch: ProtocolOptions.transferEltOptions,
        tx: ProtocolOptions.transferEltOptions,
      ) =>
    ProtocolOptions.{
      fee: Option.mapOrKeep(batch.fee, tx.fee, max),
      gasLimit: Option.mapOrKeep(batch.gasLimit, tx.gasLimit, (+)),
      storageLimit:
        Option.mapOrKeep(batch.storageLimit, tx.storageLimit, (+)),
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
              ProtocolOptions.transferEltOptions,
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
        ->Promise.value;
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
    prepareTransfers(fa12Cache, fa2Cache, source, txs, [])->Promise.all;
  };

  let batch =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~transfers:
           (ReTaquito.endpoint, PublicKeyHash.t) =>
           FutureBase.t(list(Promise.result(Toolkit.transferParams))),
        ~signingIntent,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let%Await signer = Signer.readSecretKey(source, signingIntent, baseDir);
    let provider = Toolkit.{signer: signer};
    tk->Toolkit.setProvider(provider);
    let%Await txs = endpoint->transfers(source)->Promise.map(Result.collect);
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
          ~customValues,
          ~transfers:
             (ReTaquito.endpoint, PublicKeyHash.t) =>
             FutureBase.t(list(Promise.result(Toolkit.transferParams))),
          (),
        ) => {
      let%Await alias = Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source);

      let%Await pk = Wallet.pkFromAlias(~dirpath=baseDir, ~alias);

      let tk = Toolkit.create(endpoint);
      let signer =
        EstimationSigner.create(~publicKey=pk, ~publicKeyHash=source, ());
      let provider = Toolkit.{signer: signer};
      tk->Toolkit.setProvider(provider);

      let%Await txs =
        endpoint->transfers(source)->Promise.map(Result.collect);

      let txs =
        txs->List.map(tr => {...tr, kind: opKindTransaction})->List.toArray;

      let%Await res =
        tk.estimate
        ->Toolkit.Estimation.batch(txs)
        ->ReTaquitoError.fromPromiseParsed;

      let%AwaitMap (simulations, reveal) =
        extractBatchRevealEstimation(txs, res)->Promise.value;

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
