/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

@module("@taquito/taquito") @scope("DEFAULT_FEE")
external default_fee_reveal: int = "REVEAL"

module Types = ReTaquitoTypes

module BigNumber = Types.BigNumber

type endpoint = string

module RPCClient = {
  include Types.RPCClient

  @module("@taquito/rpc") @new
  external create: endpoint => t = "RpcClient"

  @send
  external getBlockHeader: (t, unit) => Js.Promise.t<blockHeader> = "getBlockHeader"

  @send
  external getChainId: (t, unit) => Js.Promise.t<string> = "getChainId"

  @send
  external getBalance: (
    t,
    PublicKeyHash.t,
    ~params: params=?,
    unit,
  ) => Js.Promise.t<ReBigNumber.t> = "getBalance"

  @send
  external getManagerKey: (t, PublicKeyHash.t) => Js.Promise.t<Js.Nullable.t<managerKeyResult>> =
    "getManagerKey"

  @send
  external getConstants: (t, unit) => Js.Promise.t<constants> = "getConstants"
}

module Toolkit = {
  include Types.Toolkit

  module Operation = {
    include Types.Operation

    @send
    external create: (field, string) => Js.Promise.t<t> = "createOperation"

    @send
    external confirmation: (t, ~blocks: int=?) => Js.Promise.t<confirmationResult> = "confirmation"
  }

  include Types.Delegate
  include Types.Originate
  include Types.Transfer

  let prepareTransfer = (
    ~source,
    ~dest,
    ~amount,
    ~fee=?,
    ~gasLimit=?,
    ~storageLimit=?,
    ~parameter=?,
    (),
  ) => {
    kind: Operation.transactionKind,
    to_: dest,
    source: source,
    amount: amount,
    fee: fee,
    gasLimit: gasLimit,
    storageLimit: storageLimit,
    mutez: Some(true),
    parameter: parameter,
  }

  let prepareDelegate = (~source, ~delegate, ~fee=?, ()) => {
    kind: Operation.delegationKind,
    source: source,
    delegate: delegate,
    fee: fee,
  }

  let prepareOriginate = (
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
    open Types.Originate
    {
      kind: Operation.originationKind,
      source: source,
      balance: balance,
      code: code,
      storage: storage,
      delegate: delegate,
      fee: fee,
      gasLimit: gasLimit,
      storageLimit: storageLimit,
      mutez: Some(true),
    }
  }

  let makeSendParams = (~amount, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
    amount: amount,
    fee: fee,
    gasLimit: gasLimit,
    storageLimit: storageLimit,
    mutez: Some(true),
  }

  @module("@taquito/taquito") @new
  external create: endpoint => toolkit = "TezosToolkit"

  @send external setProvider: (toolkit, provider) => unit = "setProvider"

  @send
  external addExtension: (toolkit, Types.Extension.t) => unit = "addExtension"

  @send
  external transfer: (contract, transferParams) => Js.Promise.t<Types.Operation.result> = "transfer"

  @send
  external setDelegate: (contract, delegateParams) => Js.Promise.t<Types.Operation.result> =
    "setDelegate"

  @send
  external getDelegate: (tz, PublicKeyHash.t) => Js.Promise.t<Js.Nullable.t<PublicKeyHash.t>> =
    "getDelegate"

  @send
  external originate: (contract, originateParams) => Js.Promise.t<Types.Operation.result> =
    "originate"

  module Lambda = {
    include Types.Lambda
  
    @module("@taquito/taquito")
    external transferImplicit: (tz, ReBigNumber.t) => t = "transferImplicit"
  }

  module Batch = {
    include Types.Batch

    @send
    external send: t => Js.Promise.t<Types.Operation.result> = "send"

    @send external make: contract => t = "batch"

    @send
    external withTransfer: (t, transferParams) => t = "withTransfer"
    @send
    external withDelegation: (t, delegateParams) => t = "withDelegation"
    @send
    external withOrigination: (t, originateParams) => t = "withOrigination"
  }

  module Estimation = {
    include Types.Estimation

    type batchParams

    external fromTransferParams: transferParams => batchParams = "%identity"
    external fromDelegateParams: delegateParams => batchParams = "%identity"
    external fromOriginationParams: originateParams => batchParams = "%identity"

    @send
    external batch: (estimate, array<batchParams>) => Js.Promise.t<array<result>> = "batch"

    @send
    external batchOrigination: (estimate, array<originateParams>) => Js.Promise.t<array<result>> =
      "batch"

    @send
    external transfer: (estimate, transferParams) => Js.Promise.t<result> = "transfer"

    @send
    external setDelegate: (estimate, delegateParams) => Js.Promise.t<result> = "setDelegate"
  }
}

type dataBytes = {bytes: string}

@module("@taquito/michel-codec")
external unpackDataBytes: dataBytes => Js.Json.t = "unpackDataBytes"
