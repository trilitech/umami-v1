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

module Decode: {
  type address =
    | Packed(bytes)
    | Pkh(PublicKeyHash.t)

  let dataDecoder: Json.Decode.decoder<'a> => Json.Decode.decoder<array<'a>>

  let pairDecoder: (
    Json.Decode.decoder<'a>,
    Json.Decode.decoder<'b>,
  ) => Json.Decode.decoder<('a, 'b)>

  let intDecoder: Json.Decode.decoder<string>

  let bytesDecoder: Json.Decode.decoder<bytes>

  let stringDecoder: Json.Decode.decoder<string>

  let addressDecoder: Json.Decode.decoder<address>

  let fa2BalanceOfDecoder: Js.Json.t => array<((address, string), string)>
}

module MichelsonV1Expression: {
  type t = Js.Json.t
  let int: string => t
  let string: string => t
  let bytes: string => t
  let prim: (string, array<t>) => t
  let seq: array<t> => t

  module Constructors: {
    type t = Js.Json.t
    let _Pair: (t, t) => t
  }

  module Types: {
    type t = t
    let pair: (t, t) => t
    let operation: t
    let list: t => t
    let address: t
    let nat: t
    let mutez: t
    let unit: t
    let key_hash: t
    let lambda: (t, t) => t
  }

  module Instructions: {
    type t = t
    let _DROP: t
    let _NIL: Types.t => t
    let _PUSH: (Types.t, t) => t
    let _CONTRACT: Types.t => t // FIXME: can also annot endpoint
    let _IF_NONE: (t, t) => t
    let _TRANSFER_TOKENS: t
    let _CONS: t
    let _FAILWITH: t
    let _UNIT: t
    let _SOME: t
    let _NONE: Types.t => t
    let _SET_DELEGATE: t
    let _IMPLICIT_ACCOUNT: t
    let _CREATE_CONTRACT: (Types.t, Types.t, t) => t
    let _AMOUNT: t
    let _DIP: t => t
    let _PAIR: t
  }
}

module LAMBDA_PARSER: {
  type transferParams = Operation.Transaction.t

  let setDelegate: (array<Js.Json.t>, int) => option<(PublicKeyHash.t, int)>

  let removeDelegate: (array<Js.Json.t>, int) => option<int>

  let transferImplicit: (array<Js.Json.t>, int) => option<(transferParams, int)>

  let transferToContract: (array<Js.Json.t>, int) => option<(transferParams, int)>

  let transfer: (array<Js.Json.t>, int) => option<(transferParams, int)>

  let delegate: (array<Js.Json.t>, int) => option<(option<PublicKeyHash.t>, int)>

  let parseOperationsList: Js.Json.t => option<array<Operation.payload>>
}
