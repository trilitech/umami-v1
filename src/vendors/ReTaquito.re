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

  type operation = {hash: string};

  type toolkit = {
    tz,
    contract,
  };

  type provider = {signer};

  type transfer = {
    .
    "to": string,
    "source": string,
    "amount": BigNumber.t,
    "fee": option(BigNumber.t),
    "gasLimit": option(int),
    "storageLimit": option(int),
    "mutez": option(bool),
  };

  let makeTransfer =
      (~source, ~dest, ~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    {
      "to": dest,
      "source": source,
      "amount": amount,
      "fee": fee,
      "gasLimit": gasLimit,
      "storageLimit": storageLimit,
      "mutez": Some(true),
    };
  };

  [@bs.new] external create: endpoint => toolkit = "TezosToolkit";

  [@bs.send] external setProvider: (toolkit, provider) => unit = "setProvider";

  [@bs.send]
  external transfer: (contract, transfer) => Js.Promise.t(operation) =
    "transfer";
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

module Operations = {
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
          Toolkit.makeTransfer(
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
      })
    ->Future.tapOk(Js.log);
  };
};
