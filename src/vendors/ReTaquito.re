%raw
"
const { TezosToolkit, WalletOperation, OpKind, DEFAULT_FEE } =
   require('@taquito/taquito');
const { RpcClient } = require ('@taquito/rpc');
const { InMemorySigner, importKey } = require('@taquito/signer');
const BigNumber = require('bignumber.js');
";

let opKindTransaction = [%raw "OpKind.TRANSACTION"];
let default_fee_reveal = [%raw "DEFAULT_FEE.REVEAL"];

module BigNumber = {
  type t;

  [@bs.new] external fromString: string => t = "BigNumber";
  [@bs.send] external toString: t => string = "toString";

  let fromInt64 = i => i->Int64.to_string->fromString;
  let toInt64 = i => i->toString->Int64.of_string;
  let fromInt = i => i->Int.toString->fromString;
};

type error =
  | Generic(string)
  | WrongPassword;

let fromPromiseGeneric = p =>
  p->FutureJs.fromPromise(e => {
    Js.log(e);
    Generic(Js.String.make(e));
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
    (rpcClient, string, ~params: params=?, unit) => Js.Promise.t(BigNumber.t) =
    "getBalance";

  [@bs.send]
  external getManagerKey:
    (rpcClient, string) => Js.Promise.t(Js.Nullable.t(managerKeyResult)) =
    "getManagerKey";
};

let revealFee = (~endpoint, source) => {
  let client = RPCClient.create(endpoint);

  RPCClient.getManagerKey(client, source)
  ->fromPromiseGeneric
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

    [@bs.send] external create: (field, string) => t = "createOperation";

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
    amount: BigNumber.t,
    fee: option(BigNumber.t),
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
    fee: option(BigNumber.t),
  };

  let prepareDelegate = (~source, ~delegate, ~fee=?, ()) => {
    {source, delegate, fee};
  };

  type sendParams = {
    amount: BigNumber.t,
    fee: option(BigNumber.t),
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
  external getDelegate: (tz, string) => Js.Promise.t(string) = "getDelegate";

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
      (M.t, string, string, BigNumber.t) => methodResult(M.transfer) =
      "transfer";
  };

  module Estimation = {
    type result = {
      totalCost: int,
      storageLimit: int,
      gasLimit: int,
      revealFee: int,
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

module SecretAliases = {
  type alias = {
    name: string,
    value: string,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

module PkAliases = {
  type value = {
    locator: string,
    key: string,
  };

  type alias = {
    name: string,
    value,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

module PkhAliases = {
  type alias = {
    name: string,
    value: string,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

open System.Path.Ops;

let aliasFromPkh = (~dirpath, ~pkh, ()) => {
  System.File.read(dirpath / (!"public_key_hashs"))
  ->Future.mapError(e => Generic(e))
  ->Future.flatMapOk(file => {
      PkhAliases.parse(file)
      ->Js.Array2.find(a => a.value == pkh)
      ->FutureEx.fromOption(~error=Generic("No key found !"))
      ->Future.mapOk(a => a.PkhAliases.name)
    });
};

let pkFromAlias = (~dirpath, ~alias, ()) => {
  System.File.read(dirpath / (!"public_keys"))
  ->Future.mapError(e => Generic(e))
  ->Future.flatMapOk(file => {
      PkAliases.parse(file)
      ->Js.Array2.find(a => a.PkAliases.name == alias)
      ->FutureEx.fromOption(~error=Generic("No key found !"))
      ->Future.mapOk(a => a.PkAliases.value.key)
    });
};

let readSecretKey = (address, passphrase, dirpath) => {
  aliasFromPkh(~dirpath, ~pkh=address, ())
  ->Future.flatMapOk(alias => {
      System.File.read(dirpath / (!"secret_keys"))
      ->Future.mapError(e => Generic(e))
      ->Future.flatMapOk(file => {
          SecretAliases.parse(file)
          ->Js.Array2.find(a => a.SecretAliases.name == alias)
          ->FutureEx.fromOption(~error=Generic("No key found !"))
          ->Future.mapOk(a => a.SecretAliases.value)
        })
    })
  ->Future.flatMapOk(key =>
      if (key->Js.String2.startsWith("encrypted:")) {
        fromSecretKey(
          key->Js.String2.substringToEnd(~from=10),
          ~passphrase,
          (),
        )
        ->FutureJs.fromPromise(e =>
            if (Js.String.make(e)->Js.String2.includes("wrong secret key")) {
              WrongPassword;
            } else {
              Generic(Js.String.make(e));
            }
          );
      } else if (key->Js.String2.startsWith("unencrypted:")) {
        fromSecretKey(
          key->Js.String2.substringToEnd(~from=12),
          ~passphrase,
          (),
        )
        ->fromPromiseGeneric;
      } else {
        Error(Generic("Can't readkey, bad format: " ++ key))->Future.value;
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

exception Error(string);

let getDelegate = (endpoint, address) => {
  let tk = Toolkit.create(endpoint);

  Toolkit.getDelegate(tk.tz, address)
  |> Js.Promise.then_(v => Js.Promise.resolve(Some(v)))
  |> Js.Promise.catch(e =>
       if (Obj.magic(e)##status == 404) {
         Js.Promise.resolve(None);
       } else {
         Js.Promise.reject(Error(e->Js.String.make));
       }
     )
  |> (v => FutureJs.fromPromise(v, Js.String.make));
};

module Operations = {
  let confirmation = (endpoint, hash, ~blocks=?, ()) => {
    let tk = Toolkit.create(endpoint);
    let res =
      tk.operation
      ->Toolkit.Operation.create(hash)
      ->Toolkit.Operation.confirmation(~blocks?);
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

        tk.contract->Toolkit.setDelegate(dg)->fromPromiseGeneric;
      });
  };

  let batch = (~endpoint, ~baseDir, ~source, ~transfers, ~password, ()) => {
    let tk = Toolkit.create(endpoint);

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let batch: Toolkit.Batch.t = tk.contract->Toolkit.Batch.make;

        transfers(source)
        ->Array.reduce(batch, Toolkit.Batch.withTransfer)
        ->Toolkit.Batch.send
        ->fromPromiseGeneric;
      });
  };

  let transfer =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~dest,
        ~amount,
        ~password,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);

    let amount = BigNumber.fromInt64(amount);
    let fee = fee->Option.map(BigNumber.fromInt64);

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let tr =
          Toolkit.prepareTransfer(
            ~source,
            ~dest,
            ~amount,
            ~fee?,
            ~gasLimit?,
            ~storageLimit?,
            (),
          );

        tk.contract->Toolkit.transfer(tr)->fromPromiseGeneric;
      });
  };
};

let addRevealFee = (~source, ~endpoint, r) => {
  revealFee(~endpoint, source)
  ->Future.mapOk(fee =>
      Toolkit.Estimation.{...r, totalCost: fee + r.totalCost, revealFee: fee}
    );
};

module FA12Operations = {
  module Estimate = {
    let transfer =
        (
          ~endpoint,
          ~baseDir,
          ~tokenContract,
          ~source,
          ~dest,
          ~amount,
          ~fee=?,
          ~gasLimit=?,
          ~storageLimit=?,
          (),
        ) =>
      aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
      ->Future.flatMapOk(alias => pkFromAlias(~dirpath=baseDir, ~alias, ()))
      ->Future.flatMapOk(pk => {
          let tk = Toolkit.create(endpoint);

          let signer = makeDummySigner(~pk, ~pkh=source, ());
          let provider = Toolkit.{signer: signer};
          tk->Toolkit.setProvider(provider);

          tk.contract
          ->Toolkit.FA12.at(tokenContract)
          ->fromPromiseGeneric
          ->Future.mapOk(c => (c, tk));
        })
      ->Future.flatMapOk(((c, tk)) => {
          let amount = BigNumber.fromInt64(amount);
          let fee = fee->Option.map(BigNumber.fromInt64);
          let params =
            Toolkit.makeSendParams(
              ~amount=BigNumber.fromInt64(0L),
              ~fee?,
              ~gasLimit?,
              ~storageLimit?,
              (),
            );

          c.methods
          ->Toolkit.FA12.transfer(source, dest, amount)
          ->Toolkit.FA12.toTransferParams(params)
          ->(tr => tk.estimate->Toolkit.Estimation.transfer(tr))
          ->fromPromiseGeneric
          ->Future.flatMapOk(addRevealFee(~source, ~endpoint));
        });

    let batch =
        (
          ~endpoint,
          ~baseDir,
          ~source,
          ~transfers:
             string =>
             Future.t(list(Belt.Result.t(Toolkit.transferParams, error))),
          (),
        ) => {
      aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
      ->Future.flatMapOk(alias => pkFromAlias(~dirpath=baseDir, ~alias, ()))
      ->Future.mapOk(pk => {
          let tk = Toolkit.create(endpoint);
          let signer = makeDummySigner(~pk, ~pkh=source, ());
          let provider = Toolkit.{signer: signer};
          tk->Toolkit.setProvider(provider);
          tk;
        })
      ->Future.flatMapOk(tk =>
          transfers(source)
          ->Future.map(ResultEx.collect)
          ->Future.flatMapOk(txs =>
              tk.estimate
              ->Toolkit.Estimation.batch(
                  txs
                  ->List.map(tr => {...tr, kind: opKindTransaction})
                  ->List.toArray,
                )
              ->fromPromiseGeneric
            )
        )
      ->Future.flatMapOk(r =>
          revealFee(~endpoint, source)
          ->Future.mapOk(revealFee => (r, revealFee))
        );
    };
  };

  let transfer =
      (
        ~endpoint,
        ~baseDir,
        ~tokenContract,
        ~source,
        ~dest,
        ~amount,
        ~password,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);

    let amount = BigNumber.fromInt64(amount);
    let fee = fee->Option.map(BigNumber.fromInt64);

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        tk.contract->Toolkit.FA12.at(tokenContract)->fromPromiseGeneric;
      })
    ->Future.flatMapOk(c => {
        let params =
          Toolkit.makeSendParams(
            ~amount=BigNumber.fromInt64(0L),
            ~fee?,
            ~gasLimit?,
            ~storageLimit?,
            (),
          );

        c.methods
        ->Toolkit.FA12.transfer(source, dest, amount)
        ->Toolkit.FA12.send(params)
        ->fromPromiseGeneric;
      })
    ->Future.tapOk(Js.log);
  };

  type rawTransfer = {
    token: string,
    amount: BigNumber.t,
    dest: string,
    fee: option(BigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
  };

  let toRawTransfer =
      (~token, ~dest, ~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    token,
    dest,
    amount,
    fee,
    gasLimit,
    storageLimit,
  };

  let prepareTransfers:
    (_, _, _) =>
    Future.t(list(Belt.Result.t(Toolkit.transferParams, error))) =
    (transfers: list(rawTransfer), source, endpoint) => {
      let tk = Toolkit.create(endpoint);
      let contracts =
        transfers->List.reduce(Map.String.empty, (m, elt) =>
          m->Map.String.set(
            elt.token,
            tk.contract->Toolkit.FA12.at(elt.token),
          )
        );

      transfers
      ->List.map(rawTransfer => {
          let sendParams =
            Toolkit.makeSendParams(
              ~amount=BigNumber.fromInt64(0L),
              ~fee=?rawTransfer.fee,
              ~gasLimit=?rawTransfer.gasLimit,
              ~storageLimit=?rawTransfer.storageLimit,
              (),
            );

          // By construction, this exception will never be raised
          contracts
          ->Map.String.getExn(rawTransfer.token)
          ->fromPromiseGeneric
          ->Future.mapOk(c =>
              c.methods
              ->Toolkit.FA12.transfer(
                  source,
                  rawTransfer.dest,
                  rawTransfer.amount,
                )
              ->Toolkit.FA12.toTransferParams(sendParams)
            );
        })
      ->Future.all;
    };

  let batch =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~transfers:
           string =>
           Future.t(list(Belt.Result.t(Toolkit.transferParams, error))),
        ~password,
        (),
      ) => {
    let tk = Toolkit.create(endpoint);

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        transfers(source)->Future.map(ResultEx.collect);
      })
    ->Future.mapOk(txs =>
        txs->List.reduce(
          tk.contract->Toolkit.Batch.make,
          Toolkit.Batch.withTransfer,
        )
      )
    ->Future.flatMapOk(p => p->Toolkit.Batch.send->fromPromiseGeneric);
  };
};

module Estimate = {
  let transfer =
      (
        ~endpoint,
        ~baseDir,
        ~source,
        ~dest,
        ~amount,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) =>
    aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
    ->Future.flatMapOk(alias => pkFromAlias(~dirpath=baseDir, ~alias, ()))
    ->Future.flatMapOk(pk => {
        let tk = Toolkit.create(endpoint);
        let signer = makeDummySigner(~pk, ~pkh=source, ());
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let amount = BigNumber.fromInt64(amount);
        let fee = fee->Option.map(BigNumber.fromInt64);
        let tr =
          Toolkit.prepareTransfer(
            ~source,
            ~dest,
            ~amount,
            ~fee?,
            ~gasLimit?,
            ~storageLimit?,
            (),
          );
        Js.log(tr);

        tk.estimate->Toolkit.Estimation.transfer(tr)->fromPromiseGeneric;
      })
    ->Future.flatMapOk(addRevealFee(~source, ~endpoint));

  let setDelegate = (~endpoint, ~baseDir, ~source, ~delegate=?, ~fee=?, ()) =>
    aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
    ->Future.flatMapOk(alias => pkFromAlias(~dirpath=baseDir, ~alias, ()))
    ->Future.flatMapOk(pk => {
        let tk = Toolkit.create(endpoint);
        let signer = makeDummySigner(~pk, ~pkh=source, ());
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let fee = fee->Option.map(BigNumber.fromInt64);
        let sd = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());
        Js.log(sd);

        tk.estimate->Toolkit.Estimation.setDelegate(sd)->fromPromiseGeneric;
      })
    ->Future.flatMapOk(addRevealFee(~source, ~endpoint));

  let batch = (~endpoint, ~baseDir, ~source, ~transfers, ()) => {
    aliasFromPkh(~dirpath=baseDir, ~pkh=source, ())
    ->Future.flatMapOk(alias => pkFromAlias(~dirpath=baseDir, ~alias, ()))
    ->Future.flatMapOk(pk => {
        let tk = Toolkit.create(endpoint);

        let signer = makeDummySigner(~pk, ~pkh=source, ());

        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        Toolkit.Estimation.batch(tk.estimate, source->transfers)
        ->fromPromiseGeneric;
      })
    ->Future.flatMapOk(r =>
        revealFee(~endpoint, source)
        ->Future.mapOk(revealFee => (r, revealFee))
      );
  };

  let handleEstimationResults = ((results, revealFee), index) => {
    switch (index) {
    | Some(index) =>
      results
      ->Array.get(index)
      ->FutureEx.fromOption(~error=Generic("No transfer with such index"))
    | None =>
      results
      ->Array.reduce(
          Toolkit.Estimation.{
            totalCost: 0,
            gasLimit: 0,
            storageLimit: 0,
            revealFee: 0,
          },
          (
            {totalCost, gasLimit, storageLimit},
            {
              Toolkit.Estimation.totalCost: totalCost1,
              gasLimit: gasLimit1,
              storageLimit: storageLimit1,
            } as est,
          ) =>
          {
            ...est,
            totalCost: totalCost + totalCost1,
            storageLimit: storageLimit + storageLimit1,
            gasLimit: gasLimit + gasLimit1,
          }
        )
      ->(
          (Toolkit.Estimation.{gasLimit, storageLimit, totalCost}) =>
            Toolkit.Estimation.{
              gasLimit: gasLimit + 100,
              totalCost: totalCost + revealFee,
              storageLimit: storageLimit + 100,
              revealFee,
            }
        )
      ->Ok
      ->Future.value
    };
  };
};
