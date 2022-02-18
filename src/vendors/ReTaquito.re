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

[@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
external opKindTransaction: string = "TRANSACTION";

[@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
external opKindDelegation: string = "DELEGATION";

[@bs.module "@taquito/taquito"] [@bs.scope "OpKind"]
external opKindOrigination: string = "ORIGINATION";

[@bs.module "@taquito/taquito"] [@bs.scope "DEFAULT_FEE"]
external default_fee_reveal: int = "REVEAL";

module Types = ReTaquitoTypes;

module BigNumber = Types.BigNumber;

type endpoint = string;

module RPCClient = {
  include Types.RPCClient;

  [@bs.module "@taquito/rpc"] [@bs.new]
  external create: endpoint => t = "RpcClient";

  [@bs.send]
  external getBlockHeader: (t, unit) => Js.Promise.t(blockHeader) =
    "getBlockHeader";

  [@bs.send]
  external getChainId: (t, unit) => Js.Promise.t(string) = "getChainId";

  [@bs.send]
  external getBalance:
    (t, PublicKeyHash.t, ~params: params=?, unit) =>
    Js.Promise.t(ReBigNumber.t) =
    "getBalance";

  [@bs.send]
  external getManagerKey:
    (t, PublicKeyHash.t) => Js.Promise.t(Js.Nullable.t(managerKeyResult)) =
    "getManagerKey";

  [@bs.send]
  external getConstants: (t, unit) => Js.Promise.t(constants) =
    "getConstants";
};

module Toolkit = {
  include Types.Toolkit;

  module Operation = {
    include Types.Operation;

    [@bs.send]
    external create: (field, string) => Js.Promise.t(t) = "createOperation";

    [@bs.send]
    external confirmation:
      (t, ~blocks: int=?) => Js.Promise.t(confirmationResult) =
      "confirmation";
  };

  include Types.Transfer;
  include Types.Delegate;
  include Types.Originate;

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

  let prepareDelegate = (~source, ~delegate, ~fee=?, ()) => {
    Types.Delegate.{kind: opKindDelegation, source, delegate, fee};
  };

  let prepareOriginate =
      (
        ~source,
        ~balance=?,
        ~code,
        ~storage,
        ~delegate=?,
        ~fee=?,
        ~gasLimit=?,
        ~storageLimit=?,
        (),
      ) => {
    Types.Originate.{
      kind: opKindOrigination,
      source,
      balance,
      code,
      storage,
      delegate,
      fee,
      gasLimit,
      storageLimit,
      mutez: Some(true),
    };
  };

  let makeSendParams = (~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    Types.Transfer.{amount, fee, gasLimit, storageLimit, mutez: Some(true)};
  };

  [@bs.module "@taquito/taquito"] [@bs.new]
  external create: endpoint => toolkit = "TezosToolkit";

  [@bs.send] external setProvider: (toolkit, provider) => unit = "setProvider";

  [@bs.send]
  external addExtension: (toolkit, Types.Extension.t) => unit = "addExtension";

  [@bs.send]
  external transfer:
    (contract, transferParams) => Js.Promise.t(Types.Operation.result) =
    "transfer";

  [@bs.send]
  external setDelegate:
    (contract, delegateParams) => Js.Promise.t(Types.Operation.result) =
    "setDelegate";

  [@bs.send]
  external getDelegate:
    (tz, PublicKeyHash.t) => Js.Promise.t(Js.Nullable.t(PublicKeyHash.t)) =
    "getDelegate";

  [@bs.send]
  external originate:
    (contract, originateParams) => Js.Promise.t(Types.Operation.result) =
    "originate";

  module Batch = {
    include Types.Batch;

    [@bs.send]
    external send: t => Js.Promise.t(Types.Operation.result) = "send";

    [@bs.send] external make: contract => t = "batch";

    [@bs.send]
    external withTransfer: (t, transferParams) => t = "withTransfer";
  };

  module Estimation = {
    include Types.Estimation;

    [@bs.send]
    external batch:
      (estimate, array(transferParams)) => Js.Promise.t(array(result)) =
      "batch";
 
    [@bs.send]
    external batchDelegation:
      (estimate, array(delegateParams)) => Js.Promise.t(array(result)) =
      "batch";

    [@bs.send]
    external batchOrigination:
      (estimate, array(originateParams)) => Js.Promise.t(array(result)) =
      "batch";

    [@bs.send]
    external transfer: (estimate, transferParams) => Js.Promise.t(result) =
      "transfer";

    [@bs.send]
    external setDelegate: (estimate, delegateParams) => Js.Promise.t(result) =
      "setDelegate";
  };
};
