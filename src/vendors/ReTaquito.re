%raw
"
const { TezosToolkit, WalletOperation, OpKind, DEFAULT_FEE } =
   require('@taquito/taquito');
const { RpcClient } = require ('@taquito/rpc');
const { InMemorySigner, importKey } = require('@taquito/signer');
";

let opKindTransaction = [%raw "OpKind.TRANSACTION"];
let default_fee_reveal = [%raw "DEFAULT_FEE.REVEAL"];

module BigNumber = {
  let fromInt64 = i => i->Int64.to_string->ReBigNumber.fromString;
  let toInt64 = i => i->ReBigNumber.toFixed->Int64.of_string;
  let fromInt = i => i->Int.toString->ReBigNumber.fromString;
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
  type t =
    | Generic(string)
    | WrongPassword
    | UnregisteredDelegate
    | UnchangedDelegate
    | InvalidContract
    | BranchRefused
    | BadPkh;

  let parse = e =>
    switch (e.message) {
    | s when s->Js.String2.includes(wrongSecretKey) => WrongPassword
    | s when s->Js.String2.includes(branchRefused) => BranchRefused
    | s when s->Js.String2.includes(badPkh) => BadPkh
    | s when s->Js.String2.includes(unregisteredDelegate) =>
      UnregisteredDelegate
    | s when s->Js.String2.includes(unchangedDelegate) => UnchangedDelegate
    | s when s->Js.String2.includes(invalidContract) => InvalidContract
    | s => Generic(Js.String.make(s))
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
      (M.t, string, string, ReBigNumber.t) => methodResult(M.transfer) =
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
  ->Future.mapError(e => Error.Generic(e))
  ->Future.flatMapOk(file => {
      PkhAliases.parse(file)
      ->Js.Array2.find(a => a.value == pkh)
      ->FutureEx.fromOption(~error=Error.Generic("No key found !"))
      ->Future.mapOk(a => a.PkhAliases.name)
    });
};

let pkFromAlias = (~dirpath, ~alias, ()) => {
  System.File.read(dirpath / (!"public_keys"))
  ->Future.mapError(e => Error.Generic(e))
  ->Future.flatMapOk(file => {
      PkAliases.parse(file)
      ->Js.Array2.find(a => a.PkAliases.name == alias)
      ->FutureEx.fromOption(~error=Error.Generic("No key found !"))
      ->Future.mapOk(a => a.PkAliases.value.key)
    });
};

let readSecretKey = (address, passphrase, dirpath) => {
  aliasFromPkh(~dirpath, ~pkh=address, ())
  ->Future.flatMapOk(alias => {
      System.File.read(dirpath / (!"secret_keys"))
      ->Future.mapError(e => Error.Generic(e))
      ->Future.flatMapOk(file => {
          SecretAliases.parse(file)
          ->Js.Array2.find(a => a.SecretAliases.name == alias)
          ->FutureEx.fromOption(~error=Error.Generic("No key found !"))
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
              Error.WrongPassword;
            } else {
              Error.Generic(Js.String.make(e));
            }
          );
      } else if (key->Js.String2.startsWith("unencrypted:")) {
        fromSecretKey(
          key->Js.String2.substringToEnd(~from=12),
          ~passphrase,
          (),
        )
        ->fromPromiseParsed;
      } else {
        Error(Error.Generic("Can't readkey, bad format: " ++ key))
        ->Future.value;
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
  |> Js.Promise.then_(v => Js.Promise.resolve(Some(v)))
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

        tk.contract->Toolkit.setDelegate(dg)->fromPromiseParsed;
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
        ->fromPromiseParsed;
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

        tk.contract->Toolkit.transfer(tr)->fromPromiseParsed;
      });
  };
};

let addRevealFee = (~source, ~endpoint, r) => {
  revealFee(~endpoint, source)
  ->Future.mapOk(fee =>
      Toolkit.Estimation.{...r, totalCost: fee + r.totalCost, revealFee: fee}
    );
};

let handleCustomOptions = (results, (fee, storageLimit, gasLimit)) => {
  ...results,
  Toolkit.Estimation.totalCost:
    fee->Option.getWithDefault(results.Toolkit.Estimation.totalCost),
  storageLimit: storageLimit->Option.getWithDefault(results.storageLimit),
  gasLimit: gasLimit->Option.getWithDefault(results.gasLimit),
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
          ->fromPromiseParsed
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
          ->fromPromiseParsed
          ->Future.flatMapOk(addRevealFee(~source, ~endpoint))
          ->Future.mapOk(res =>
              res->handleCustomOptions((
                fee->Option.map(f => f->BigNumber.toInt64->Int64.to_int),
                storageLimit,
                gasLimit,
              ))
            );
        });

    let batch =
        (
          ~endpoint,
          ~baseDir,
          ~source,
          ~transfers:
             string =>
             Future.t(list(Belt.Result.t(Toolkit.transferParams, Error.t))),
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
              ->fromPromiseParsed
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

        tk.contract->Toolkit.FA12.at(tokenContract)->fromPromiseParsed;
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
        ->fromPromiseParsed;
      })
    ->Future.tapOk(Js.log);
  };

  type rawTransfer = {
    token: string,
    amount: ReBigNumber.t,
    dest: string,
    fee: option(ReBigNumber.t),
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
    Future.t(list(Belt.Result.t(Toolkit.transferParams, Error.t))) =
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
          ->fromPromiseParsed
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
           Future.t(list(Belt.Result.t(Toolkit.transferParams, Error.t))),
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
    ->Future.flatMapOk(p => p->Toolkit.Batch.send->fromPromiseParsed);
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

        tk.estimate->Toolkit.Estimation.transfer(tr)->fromPromiseParsed;
      })
    ->Future.flatMapOk(addRevealFee(~source, ~endpoint))
    ->Future.mapOk(res =>
        res->handleCustomOptions((
          fee->Option.map(Int64.to_int),
          storageLimit,
          gasLimit,
        ))
      );

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

        tk.estimate->Toolkit.Estimation.setDelegate(sd)->fromPromiseParsed;
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
        ->fromPromiseParsed;
      })
    ->Future.flatMapOk(r =>
        revealFee(~endpoint, source)
        ->Future.mapOk(revealFee => (r, revealFee))
      );
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
          },
          (
            {
              totalCost,
              gasLimit,
              storageLimit,
              minimalFeeMutez,
              suggestedFeeMutez,
              burnFeeMutez,
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
            };
          },
        )
      ->(
          (Toolkit.Estimation.{gasLimit, storageLimit, totalCost} as est) =>
            Toolkit.Estimation.{
              ...est,
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
