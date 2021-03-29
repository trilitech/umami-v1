%raw
"
const { TezosToolkit, WalletOperation, OpKind } = require('@taquito/taquito');
const { RpcClient } = require ('@taquito/rpc');
const { InMemorySigner, importKey } = require('@taquito/signer');
const BigNumber = require('bignumber.js');
";

module BigNumber = {
  type t;

  [@bs.new] external fromString: string => t = "BigNumber";

  let fromInt64 = i => i->Int64.to_string->fromString;
};

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

  [@bs.send]
  external getBalance:
    (rpcClient, string, ~params: params=?, unit) => Js.Promise.t(int) =
    "getBalance";
};

module Toolkit = {
  type tz;
  type contract;

  type operationResult = {hash: string};

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
  };

  type provider = {signer};

  type transferParams = {
    [@bs.as "to"]
    to_: string,
    source: string,
    amount: BigNumber.t,
    fee: option(BigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
  };

  let prepareTransfer =
      (~source, ~dest, ~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    {
      to_: dest,
      source,
      amount,
      fee,
      gasLimit,
      storageLimit,
      mutez: Some(true),
    };
  };

  type delegateParams = {
    source: string,
    delegate: option(string),
    fee: option(int),
  };

  let prepareDelegate = (~source, ~delegate, ~fee=?, ()) => {
    {source, delegate, fee};
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

  module Batch = {
    type t;

    [@bs.send] external send: t => Js.Promise.t(operationResult) = "send";

    [@bs.send] external make: contract => t = "batch";

    [@bs.send]
    external withTransfer: (t, transferParams) => t = "withTransfer";
  };
};

module Balance = {
  let get = (endpoint, ~address, ~params=?, ()) => {
    RPCClient.create(endpoint)
    ->RPCClient.getBalance(address, ~params?, ())
    ->FutureJs.fromPromise(Js.String.make);
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

module PkhAliases = {
  type alias = {
    name: string,
    value: string,
  };
  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
};

let aliasFromPkh = (dirname, pkh) => {
  System.File.read(dirname ++ "/public_key_hashs")
  ->Future.flatMapOk(file => {
      PkhAliases.parse(file)
      ->Js.Array2.find(a => a.value == pkh)
      ->FutureEx.fromOption(~error="No key found !")
      ->Future.mapOk(a => a.PkhAliases.name)
    });
};

let readSecretKey = (address, passphrase, dirname) => {
  aliasFromPkh(dirname, address)
  ->Future.flatMapOk(alias => {
      System.File.read(dirname ++ "/secret_keys")
      ->Future.flatMapOk(file => {
          SecretAliases.parse(file)
          ->Js.Array2.find(a => a.SecretAliases.name == alias)
          ->FutureEx.fromOption(~error="No key found !")
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
        ->FutureJs.fromPromise(Js.String.make);
      } else if (key->Js.String2.startsWith("unencrypted:")) {
        fromSecretKey(
          key->Js.String2.substringToEnd(~from=12),
          ~passphrase,
          (),
        )
        ->FutureJs.fromPromise(Js.String.make);
      } else {
        Error("Can't readkey, bad format: " ++ key)->Future.value;
      }
    );
};

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

    readSecretKey(source, password, baseDir)
    ->Future.flatMapOk(signer => {
        let provider = Toolkit.{signer: signer};
        tk->Toolkit.setProvider(provider);

        let dg = Toolkit.prepareDelegate(~source, ~delegate, ~fee?, ());

        tk.contract
        ->Toolkit.setDelegate(dg)
        ->FutureJs.fromPromise(e => {
            Js.log(e);
            Js.String.make(e);
          });
      })
    ->Future.tapOk(Js.log);
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
        ->FutureJs.fromPromise(e => {
            Js.log(e);
            Js.String.make(e);
          });
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

        tk.contract
        ->Toolkit.transfer(tr)
        ->FutureJs.fromPromise(e => {
            Js.log(e);
            Js.String.make(e);
          });
      });
  };
};
