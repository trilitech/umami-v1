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

module Types = ReTaquitoTypes;

module BigNumber = Types.BigNumber;

/* A bare contract is represented by three methods:
       - `at`, which fetch the contract and builds its abstraction;
       - `send`, that sends the result of an entrypoint to the contract
       - `toTransferParams`, that generates the transferParam from an entrypoint
     call
   */
module Contract = (ContractAbstraction: Types.ContractAbstraction) => {
  module Abstraction = ContractAbstraction;
  include Types.Contract;

  [@bs.send]
  external at:
    (Types.Toolkit.contract, PublicKeyHash.t) => Js.Promise.t(Abstraction.t) =
    "at";

  [@bs.send]
  external send:
    (methodResult(_), Types.Transfer.sendParams) =>
    Js.Promise.t(Types.Operation.result) =
    "send";

  [@bs.send]
  external toTransferParams:
    (methodResult(_), Types.Transfer.sendParams) =>
    Types.Transfer.transferParams =
    "toTransferParams";
};

module rec AnyAbstraction: Types.ContractAbstraction = AnyAbstraction;

module FA12Abstraction = {
  type transfer;

  type methods = {
    /* Calling it doesn't work since it need to be an external */
    transfer:
      (PublicKeyHash.t, PublicKeyHash.t, BigNumber.fixed) =>
      Types.Contract.methodResult(transfer),
  };
  type storage;
  type entrypoints;
  type t = {
    address: PublicKeyHash.t,
    entrypoints,
    methods,
  };
};

module FA12 = {
  include Contract(FA12Abstraction);

  [@bs.send]
  external transfer:
    (Abstraction.methods, PublicKeyHash.t, PublicKeyHash.t, BigNumber.fixed) =>
    methodResult(FA12Abstraction.transfer) =
    "transfer";

  let transfer = (c: Abstraction.t, src, dst, amount) => {
    transfer(c.methods, src, dst, amount);
  };
};
