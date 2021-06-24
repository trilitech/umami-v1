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

%raw
"
const { TezosToolkit, WalletOperation, OpKind, DEFAULT_FEE } =
   require('@taquito/taquito');
const { RpcClient } = require ('@taquito/rpc');
const { InMemorySigner, importKey } = require('@taquito/signer');
";

let opKindTransaction = [%raw "OpKind.TRANSACTION"];
let default_fee_reveal = [%raw "DEFAULT_FEE.REVEAL"];

module BigNumber: {
  type fixed;
  let toFixed: ReBigNumber.t => fixed;
  let fromInt64: Int64.t => ReBigNumber.t;
  let toInt64: ReBigNumber.t => Int64.t;
} = {
  type fixed = string;

  let toFixed = ReBigNumber.toFixed;

  let fromInt64 = ReBigNumber.fromInt64;
  let toInt64 = ReBigNumber.toInt64;
};

module Error = {
  type raw = {message: string};

  let toRaw: Js.Promise.error => raw = Obj.magic;

  let branchRefused = "branch refused";
  let wrongSecretKey = "wrong secret key";
  let badPkh = "Unexpected data (Signature.Public_key_hash)";
  let unregisteredDelegate = "contract.manager.unregistered_delegate";
  let unchangedDelegate = "contract.manager.delegate.unchanged";
  let invalidContract = "Invalid contract notation";
  let emptyTransaction = "contract.empty_transaction";

  type t =
    | Generic(string)
    | WrongPassword
    | UnregisteredDelegate
    | UnchangedDelegate
    | EmptyTransaction
    | InvalidContract
    | BranchRefused
    | BadPkh
    | WalletError(Wallet.error);

  let parse = e =>
    switch (e.message) {
    | s when s->Js.String2.includes(wrongSecretKey) => WrongPassword
    | s when s->Js.String2.includes(branchRefused) => BranchRefused
    | s when s->Js.String2.includes(badPkh) => BadPkh
    | s when s->Js.String2.includes(unregisteredDelegate) =>
      UnregisteredDelegate
    | s when s->Js.String2.includes(unchangedDelegate) => UnchangedDelegate
    | s when s->Js.String2.includes(invalidContract) => InvalidContract
    | s when s->Js.String2.includes(emptyTransaction) => EmptyTransaction
    | s => Generic(Js.String.make(s))
    };
};

module Utils = {
  type addressValidityError = [
    | `No_prefix_matched
    | `Invalid_checksum
    | `Invalid_length
    | `UnknownError(int)
  ];

  [@bs.module "@taquito/utils"]
  external validateAddressRaw: string => int = "validateAddress";

  [@bs.module "@taquito/utils"]
  external validateContractAddressRaw: string => int =
    "validateContractAddress";

  let handleValidity =
    fun
    | 0 => `No_prefix_matched
    | 1 => `Invalid_checksum
    | 2 => `Invalid_length
    | 3 => `Valid
    | n => `UnknownError(n);

  let validateAddress = s =>
    switch (s->validateAddressRaw->handleValidity) {
    | `Valid => Ok(`Address)
    | #addressValidityError as err => Error(err)
    };

  let validateContractAddress = s =>
    switch (s->validateContractAddressRaw->handleValidity) {
    | `Valid => Ok(`Contract)
    | #addressValidityError as err => Error(err)
    };

  let validateAnyAddress = s =>
    switch (s->validateContractAddress) {
    | Ok(`Contract) => Ok(`Contract)
    | Error(_) =>
      switch (s->validateAddress) {
      | Ok(`Address) => Ok(`Address)
      | Error(#addressValidityError as err) => Error(err)
      }
    };
};

let fromPromiseParsed = p =>
  p->FutureJs.fromPromise(e => {
    let e = e->Error.toRaw;
    Js.log(e.Error.message);

    e->Error.parse;
  });

let walletOperation = [%raw "WalletOperation"];
let opKind = [%raw "OpKind"];

let rpcClient = [%raw "RpcClient"];
let inMemorySigner = [%raw "InMemorySigner"];

type signer;
type rpcClient;

[@bs.val] [@bs.scope "InMemorySigner"]
external fromSecretKey:
  (string, ~passphrase: string=?, unit) => Js.Promise.t(signer) =
  "fromSecretKey";

type endpoint = string;

module RPCClient = {
  [@bs.new] external create: endpoint => rpcClient = "RpcClient";

  type params = {block: string};
  type managerKeyResult = {key: string};

  [@bs.send]
  external getBalance:
    (rpcClient, string, ~params: params=?, unit) =>
    Js.Promise.t(ReBigNumber.t) =
    "getBalance";

  [@bs.send]
  external getManagerKey:
    (rpcClient, string) => Js.Promise.t(Js.Nullable.t(managerKeyResult)) =
    "getManagerKey";
};

let revealFee = (~endpoint, source) => {
  let client = RPCClient.create(endpoint);

  RPCClient.getManagerKey(client, source)
  ->fromPromiseParsed
  ->Future.mapOk(k => Js.Nullable.isNullable(k) ? default_fee_reveal : 0);
};

module Toolkit = {
  type tz;
  type contract;
  type estimate;

  type operationResult = {hash: string};
  type transferMichelsonParameter;

  module Operation = {
    type field;
    type t;

    type block = {hash: string};

    type confirmationResult = {block};

    [@bs.send]
    external create: (field, string) => Js.Promise.t(t) = "createOperation";

    [@bs.send]
    external confirmation:
      (t, ~blocks: int=?) => Js.Promise.t(confirmationResult) =
      "confirmation";
  };

  type toolkit = {
    tz,
    contract,
    operation: Operation.field,
    estimate,
  };

  type provider = {signer};

  type transferParams = {
    kind: string,
    [@bs.as "to"]
    to_: string,
    source: string,
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
    parameter: option(transferMichelsonParameter),
  };

  let prepareTransfer =
      (~source, ~dest, ~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    {
      kind: opKindTransaction,
      to_: dest,
      source,
      amount,
      fee,
      gasLimit,
      storageLimit,
      mutez: Some(true),
      parameter: None,
    };
  };

  type delegateParams = {
    source: string,
    delegate: option(string),
    fee: option(ReBigNumber.t),
  };

  let prepareDelegate = (~source, ~delegate, ~fee=?, ()) => {
    {source, delegate, fee};
  };

  type sendParams = {
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
  };

  let makeSendParams = (~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    {amount, fee, gasLimit, storageLimit, mutez: Some(true)};
  };

  [@bs.new] external create: endpoint => toolkit = "TezosToolkit";

  [@bs.send] external setProvider: (toolkit, provider) => unit = "setProvider";

  [@bs.send]
  external transfer:
    (contract, transferParams) => Js.Promise.t(operationResult) =
    "transfer";

  [@bs.send]
  external setDelegate:
    (contract, delegateParams) => Js.Promise.t(operationResult) =
    "setDelegate";

  [@bs.send]
  external getDelegate: (tz, string) => Js.Promise.t(Js.Nullable.t(string)) =
    "getDelegate";

  module type METHODS = {type t;};

  module Contract = (M: METHODS) => {
    type contractAbstraction = {methods: M.t};

    type methodResult('meth);

    [@bs.send]
    external at: (contract, string) => Js.Promise.t(contractAbstraction) =
      "at";

    [@bs.send]
    external send:
      (methodResult(_), sendParams) => Js.Promise.t(operationResult) =
      "send";

    [@bs.send]
    external toTransferParams: (methodResult(_), sendParams) => transferParams =
      "toTransferParams";
  };

  module Batch = {
    type t;

    [@bs.send] external send: t => Js.Promise.t(operationResult) = "send";

    [@bs.send] external make: contract => t = "batch";

    [@bs.send]
    external withTransfer: (t, transferParams) => t = "withTransfer";
  };

  module FA12Methods = {
    type t;

    type transfer;
  };

  module FA12 = {
    module M = FA12Methods;
    include Contract(M);

    [@bs.send]
    external transfer:
      (M.t, string, string, BigNumber.fixed) => methodResult(M.transfer) =
      "transfer";
  };

  module Estimation = {
    type result = {
      totalCost: int,
      storageLimit: int,
      gasLimit: int,
      revealFee: int,
      minimalFeeMutez: int,
      suggestedFeeMutez: int,
      burnFeeMutez: int,
      customFeeMutez: int,
    };

    [@bs.send]
    external batch:
      (estimate, array(transferParams)) => Js.Promise.t(array(result)) =
      "batch";

    [@bs.send]
    external transfer: (estimate, transferParams) => Js.Promise.t(result) =
      "transfer";

    [@bs.send]
    external setDelegate: (estimate, delegateParams) => Js.Promise.t(result) =
      "setDelegate";
  };
};

module Balance = {
  let get = (endpoint, ~address, ~params=?, ()) => {
    RPCClient.create(endpoint)
    ->RPCClient.getBalance(address, ~params?, ())
    ->FutureJs.fromPromise(e => {
        Js.log(e);
        Js.String.make(e);
      })
    ->Future.mapOk(BigNumber.toInt64);
  };
};

let convertWalletError = res =>
  res->ResultEx.mapError(
    fun
    | Wallet.Generic(e) => Error(Error.Generic(e))
    | e => Error(Error.WalletError(e)),
  );

let readEncryptedKey = (key, passphrase) =>
  fromSecretKey(key->Js.String2.substringToEnd(~from=10), ~passphrase, ())
  ->FutureJs.fromPromise(e =>
      if (Js.String.make(e)->Js.String2.includes("wrong secret key")) {
        Error.WrongPassword;
      } else {
        Error.Generic(Js.String.make(e));
      }
    );

let readUnencryptedKey = key =>
  fromSecretKey(key->Js.String2.substringToEnd(~from=12), ~passphrase="", ())
  ->fromPromiseParsed;

let readSecretKey = (address, passphrase, dirpath) => {
  Wallet.readSecret(address, dirpath)
  ->Future.map(convertWalletError)
  ->Future.flatMapOk(((kind, key)) =>
      switch (kind) {
      | Encrypted => readEncryptedKey(key, passphrase)
      | Unencrypted => readUnencryptedKey(key)
      | Ledger => Future.value(Error(Error.Generic("Ledger not supported")))
      }
    );
};

%raw
"
class NoopSigner {
  constructor(pk, pkh) {
    this.pk = pk;
    this.pkh = pkh;
  }
  async publicKey() {
    return this.pk;
  }
  async publicKeyHash() {
    return this.pkh;
  }
  async secretKey() {
    throw new UnconfiguredSignerError();
  }
  async sign(_bytes, _watermark) {
    throw new UnconfiguredSignerError();
  }
}
";

[@bs.new]
external makeDummySigner: (~pk: string, ~pkh: string, unit) => signer =
  "NoopSigner";

exception RejectError(string);

let getDelegate = (endpoint, address) => {
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
  |> (v => FutureJs.fromPromise(v, Js.String.make));
};

module Operations = {
  let confirmation = (endpoint, hash, ~blocks=?, ()) => {
    let tk = Toolkit.create(endpoint);
    let res =
      tk.operation->Toolkit.Operation.create(hash)
      |> Js.Promise.then_(op => op->Toolkit.Operation.confirmation(~blocks?));
    res->FutureJs.fromPromise(e => {
      Js.log(e);
      Js.String.make(e);
    });
  };

  let setDelegate =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~delegate: option(string),
        ~password,
        ~fee=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);
    let fee = fee->Option.map(BigNumber.fromInt64);

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let dg = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());

        tk.contract->Toolkit.setDelegate(dg)->fromPromiseParsed;
      });
  };
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
    fee->Option.getWithDefault(results.Toolkit.Estimation.suggestedFeeMutez),
  storageLimit: storageLimit->Option.getWithDefault(results.storageLimit),
  gasLimit: gasLimit->Option.getWithDefault(results.gasLimit),
};

module Estimate = {
  let setDelegate = (~endpoint, ~baseDir, ~source, ~delegate=?, ~fee=?, ()) =>
    Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
    ->Future.flatMapOk(alias =>
        Wallet.pkFromAlias(~dirpath=baseDir, ~alias, ())
      )
    ->Future.map(convertWalletError)
    ->Future.flatMapOk(pk => {
        let tk = Toolkit.create(endpoint);
        let signer = makeDummySigner(~pk, ~pkh=source, ());
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let fee = fee->Option.map(ProtocolXTZ.toBigNumber);
        let sd = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());
        Js.log(sd);

        tk.estimate->Toolkit.Estimation.setDelegate(sd)->fromPromiseParsed;
      })
    ->Future.mapOk(res =>
        res->handleCustomOptions((
          fee->Option.map(ProtocolXTZ.unsafeToMutezInt),
          None,
          None,
        ))
      )
    ->Future.flatMapOk(addRevealFee(~source, ~endpoint));

  let handleEstimationResults = ((results, revealFee), options, index) => {
    switch (index) {
    | Some(index) =>
      let customOptions =
        options[index]->Option.getWithDefault((None, None, None));
      Js.log(customOptions);
      results
      ->Array.get(index)
      ->FutureEx.fromOption(
          ~error=Error.Generic("No transfer with such index"),
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
};

module Transfer = {
  module ContractCache = {
    type t = {
      contracts:
        MutableMap.String.t(Js.Promise.t(Toolkit.FA12.contractAbstraction)),
      toolkit: Toolkit.toolkit,
    };

    let make = toolkit => {contracts: MutableMap.String.make(), toolkit};

    let findContract = (cache, token) =>
      switch (MutableMap.String.get(cache.contracts, token)) {
      | Some(c) => c
      | None =>
        let c = cache.toolkit.contract->Toolkit.FA12.at(token);
        cache.contracts->MutableMap.String.set(token, c);
        c;
      };

    let clear = cache => cache.contracts->MutableMap.String.clear;
  };

  let prepareFA12Transfer =
      (
        contractCache,
        ~source,
        ~token,
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
    ->fromPromiseParsed
    ->Future.mapOk(c =>
        c.methods
        ->Toolkit.FA12.transfer(source, dest, amount->BigNumber.toFixed)
        ->Toolkit.FA12.toTransferParams(sendParams)
      );
  };

  let prepareTransfer = Toolkit.prepareTransfer;

  let prepareTransfers = (txs, contractCache, source) =>
    txs
    ->List.map((tx: Transfer.elt) =>
        switch (tx.amount) {
        | XTZ(amount) =>
          prepareTransfer(
            ~source,
            ~dest=tx.destination,
            ~amount=amount->ProtocolXTZ.toBigNumber,
            ~fee=?tx.tx_options.fee->Option.map(ProtocolXTZ.toBigNumber),
            ~gasLimit=?tx.tx_options.gasLimit,
            ~storageLimit=?tx.tx_options.storageLimit,
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
            ~fee=?tx.tx_options.fee->Option.map(ProtocolXTZ.toBigNumber),
            ~gasLimit=?tx.tx_options.gasLimit,
            ~storageLimit=?tx.tx_options.storageLimit,
            (),
          )
        }
      )
    ->Future.all;

  module Estimate = {
    let batch =
        (
          ~endpoint,
          ~baseDir,
          ~source,
          ~transfers:
             (ContractCache.t, string) =>
             Future.t(list(Belt.Result.t(Toolkit.transferParams, Error.t))),
          (),
        ) => {
      Wallet.aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
      ->Future.flatMapOk(alias =>
          Wallet.pkFromAlias(~dirpath=baseDir, ~alias, ())
        )
      ->Future.map(convertWalletError)
      ->Future.mapOk(pk => {
          let tk = Toolkit.create(endpoint);
          let signer = makeDummySigner(~pk, ~pkh=source, ());
          let provider = Toolkit.{signer: signer};
          tk->Toolkit.setProvider(provider);
          tk;
        })
      ->Future.flatMapOk(tk =>
          ContractCache.make(tk)
          ->transfers(source)
          ->Future.map(ResultEx.collect)
          ->Future.flatMapOk(txs =>
              tk.estimate
              ->Toolkit.Estimation.batch(
                  txs
                  ->List.map(tr => {...tr, kind: opKindTransaction})
                  ->List.toArray,
                )
              ->fromPromiseParsed
            )
        )
      ->Future.flatMapOk(r =>
          revealFee(~endpoint, source)
          ->Future.mapOk(revealFee => (r, revealFee))
        );
    };
  };

  let batch =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~transfers:
           (ContractCache.t, string) =>
           Future.t(list(Belt.Result.t(Toolkit.transferParams, Error.t))),
        ~password,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);

    readSecretKey(source, password, baseDir)
    ->Future.mapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);
        tk;
      })
    ->Future.flatMapOk(tk =>
        ContractCache.make(tk)
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
        ->fromPromiseParsed;
      });
  };
};
