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

module TransactionParameters = {
  type entrypoint = ReTaquitoTypes.Transfer.Parameters.entrypoint;

  // This type cannot be build and destructed except from bindings modules
  // ReBeacon and ReTaquito, hence its abstract nature.
  module MichelineMichelsonV1Expression = {
    type t = ReTaquitoTypes.Micheline.t;

    let toString = c =>
      c
      ->Js.Json.stringifyAny
      ->Option.map(Js.Json.parseExn)
      ->Option.map(j => Js.Json.stringifyWithSpace(j, 4));

    let parseMicheline = s =>
      try(
        ReTaquitoParser.parser()
        ->ReTaquitoParser.parseMichelineExpression(s)
        ->Ok
      ) {
      | Js.Exn.Error(exn) =>
        Error(ReTaquitoError.ParseMicheline(exn->Js.Exn.message))
      };

    let parseScript = s =>
      try(ReTaquitoParser.parser()->ReTaquitoParser.parseScript(s)->Ok) {
      | Js.Exn.Error(exn) =>
        Error(ReTaquitoError.ParseScript(exn->Js.Exn.message))
      };
  };

  type t = ReTaquitoTypes.Transfer.Parameters.t;
};

type transferEltOptions = {
  fee: option(Tez.t),
  gasLimit: option(int),
  storageLimit: option(int),
  parameter: option(TransactionParameters.MichelineMichelsonV1Expression.t),
  entrypoint: option(TransactionParameters.entrypoint),
};

let txOptionsSet = telt =>
  telt.fee != None || telt.gasLimit != None || telt.storageLimit != None;

type delegationOptions = {
  fee: option(Tez.t),
  burnCap: option(Tez.t),
  forceLowFee: option(bool),
};

let delegationOptionsSet = (dopt: delegationOptions) => dopt.fee != None;

type originationOptions = {
  fee: option(Tez.t),
  burnCap: option(Tez.t),
  forceLowFee: option(bool),
};

let originationOptionsSet = (oopt: originationOptions) => oopt.fee != None;

type transferOptions = {
  burnCap: option(Tez.t),
  forceLowFee: option(bool),
};

let makeTransferEltOptions =
    (~fee=?, ~gasLimit=?, ~storageLimit=?, ~parameter=?, ~entrypoint=?, ()) => {
  fee,
  gasLimit,
  storageLimit,
  parameter,
  entrypoint,
};

let makeDelegationOptions:
  (
    ~fee: option(Tez.t),
    ~burnCap: option(Tez.t),
    ~forceLowFee: option(bool),
    unit
  ) =>
  delegationOptions =
  (~fee, ~burnCap, ~forceLowFee, ()) => {fee, burnCap, forceLowFee};

let makeOriginationOptions:
  (
    ~fee: option(Tez.t),
    ~burnCap: option(Tez.t),
    ~forceLowFee: option(bool),
    unit
  ) =>
  originationOptions =
  (~fee, ~burnCap, ~forceLowFee, ()) => {fee, burnCap, forceLowFee};

let makeTransferOptions = (~burnCap=?, ~forceLowFee=?, ()) => {
  burnCap,
  forceLowFee,
};

let emptyTransferOptions = {
  fee: None,
  gasLimit: None,
  storageLimit: None,
  parameter: None,
  entrypoint: None,
};
