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

let walletOperation = [%raw "WalletOperation"];
let opKind = [%raw "OpKind"];

let rpcClient = [%raw "RpcClient"];
type rpcClient;

type endpoint = string;

module RPCClient = {
  [@bs.new] external create: endpoint => rpcClient = "RpcClient";

  type params = {block: string};
  type managerKeyResult = {key: string};

  [@bs.send]
  external getBalance:
    (rpcClient, PublicKeyHash.t, ~params: params=?, unit) =>
    Js.Promise.t(ReBigNumber.t) =
    "getBalance";

  [@bs.send]
  external getManagerKey:
    (rpcClient, PublicKeyHash.t) =>
    Js.Promise.t(Js.Nullable.t(managerKeyResult)) =
    "getManagerKey";
};

module Toolkit = {
  type tz;
  type contract;
  type estimate;

  type operationResult = {hash: string};

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

  type provider = {signer: ReTaquitoSigner.t};

  type transferParams = {
    kind: string,
    [@bs.as "to"]
    to_: PublicKeyHash.t,
    source: PublicKeyHash.t,
    amount: ReBigNumber.t,
    fee: option(ReBigNumber.t),
    gasLimit: option(int),
    storageLimit: option(int),
    mutez: option(bool),
    parameter: option(ProtocolOptions.TransactionParameters.t),
  };

  let prepareTransfer =
      (
        ~source,
        ~dest,
        ~amount,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        ~parameter=?,
        (),
      ) => {
    {
      kind: opKindTransaction,
      to_: dest,
      source,
      amount,
      fee,
      gasLimit,
      storageLimit,
      mutez: Some(true),
      parameter,
    };
  };

  type delegateParams = {
    source: PublicKeyHash.t,
    delegate: option(PublicKeyHash.t),
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
  external getDelegate:
    (tz, PublicKeyHash.t) => Js.Promise.t(Js.Nullable.t(PublicKeyHash.t)) =
    "getDelegate";

  module type METHODS = {type t;};

  module Contract = (M: METHODS) => {
    type contractAbstraction = {methods: M.t};

    type methodResult('meth);

    [@bs.send]
    external at:
      (contract, PublicKeyHash.t) => Js.Promise.t(contractAbstraction) =
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
      (M.t, PublicKeyHash.t, PublicKeyHash.t, BigNumber.fixed) =>
      methodResult(M.transfer) =
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
