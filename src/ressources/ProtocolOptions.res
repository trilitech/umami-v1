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

module TransactionParameters = {
  // This type cannot be build and destructed except from bindings modules
  // ReBeacon and ReTaquito, hence its abstract nature.
  module MichelineMichelsonV1Expression = {
    let toString = c =>
      c
      ->Js.Json.stringifyAny
      ->Option.map(Js.Json.parseExn)
      ->Option.map(j => Js.Json.stringifyWithSpace(j, 4))

    let parseMicheline = s =>
      try ReTaquitoParser.parser()->ReTaquitoParser.parseMichelineExpression(s)->Ok catch {
      | Js.Exn.Error(exn) => Error(ReTaquitoError.ParseMicheline(exn->Js.Exn.message))
      }

  }

  type entrypoint = ReTaquitoTypes.Transfer.Entrypoint.t
  type value = ReTaquitoTypes.MichelsonV1Expression.t

  let getEntrypoint = x =>
    switch x {
    | Some(e) => e
    | None => ReTaquitoTypes.Transfer.Entrypoint.default
    }

  let getValue = x =>
    switch x {
    | Some(e) => e
    | None => ReTaquitoTypes.MichelsonV1Expression.unit
    }

}

type parameter = {
  entrypoint: option<TransactionParameters.entrypoint>,
  value: option<TransactionParameters.value>,
}

type t = {
  fee: option<Tez.t>,
  gasLimit: option<int>,
  storageLimit: option<int>,
}

let txOptionsSet = telt => telt.fee != None || (telt.gasLimit != None || telt.storageLimit != None)

let makeParameter = (~entrypoint=?, ~value=?, ()) => {
  value: value,
  entrypoint: entrypoint,
}

let make = (~fee=?, ~gasLimit=?, ~storageLimit=?, ()) => {
  fee: fee,
  gasLimit: gasLimit,
  storageLimit: storageLimit,
}
