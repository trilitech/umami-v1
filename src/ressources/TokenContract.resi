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

open TokenRepr

type Errors.t +=
  | UnknownKind(string)

type kind = [#KFA1_2 | #KFA2]

type t = {
  address: address,
  kind: kind,
}

type tzktTokenBalance = {
  token: t,
}

let fromTokenKind: TokenRepr.kind => kind
let toTokenKind: (kind, int) => TokenRepr.kind
let kindToString: kind => string

module Decode: {
  let kindFromString: string => Promise.result<kind>
  let kindDecoder: Js.Json.t => kind
  let record: Js.Json.t => t
  let array: Js.Json.t => array<t>
  let map: Js.Json.t => PublicKeyHash.Map.map<t>
  let tzktDecoder: Js.Json.t => tzktTokenBalance
}

module Encode: {
  let kindEncoder: kind => string
  let record: t => Js.Json.t
  let array: array<t> => Js.Json.t
}
