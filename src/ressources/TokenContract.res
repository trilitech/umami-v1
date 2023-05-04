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

let () = Errors.registerHandler("Wallet", x =>
  switch x {
  | UnknownKind(k) => I18n.Errors.unknown_kind(k)->Some
  | _ => None
  }
)

type kind = [#KFA1_2 | #KFA2]

type t = {
  address: address,
  kind: kind,
}

type tzktTokenBalance = {
  token: t,
}

let fromTokenKind = x =>
  switch x {
  | TokenRepr.FA1_2 => #KFA1_2
  | FA2(_) => #KFA2
  }

let toTokenKind = (kind, id) =>
  switch kind {
  | #KFA1_2 => TokenRepr.FA1_2
  | #KFA2 => FA2(id)
  }

let kindToString = x =>
  switch x {
  | #KFA1_2 => "FA1.2"
  | #KFA2 => "FA2"
  }

module Decode = {
  let kindFromString = x =>
    switch x {
    | "fa1-2"
    // compatibility with TZKT API
    | "fa1.2" =>
      Ok(#KFA1_2)
    | "fa2" => Ok(#KFA2)
    | k => Error(UnknownKind(k))
    }

  let kindDecoder = json =>
    {
      open Json.Decode
      json |> field("kind", string)
    }
    ->kindFromString
    ->Result.getExn

  let record = json => {
    open Json.Decode
    {
      kind: kindDecoder(json),
      address: json |> either(
        field("address", PublicKeyHash.decoder),
        field("k", PublicKeyHash.decoder),
      ),
    }
  }

  let array = json => json |> Json.Decode.array(record)
  let map = json =>
    json
    ->array
    ->Array.reduce(PublicKeyHash.Map.empty, (contracts, t) =>
      contracts->PublicKeyHash.Map.set(t.address, t)
    )

  type address = {
    address: PublicKeyHash.t
  }

  let addressDecoder = json => {
    open Json.Decode
    {address: json |> field("address", PublicKeyHash.decoder)}
  }

  let tokenDecoder = json => {
    open Json.Decode
    {
      address: (json |> field("contract", addressDecoder)).address,
      kind: json |> field("standard", string) |> kindFromString |> Result.getExn
    }
  }

  let tzktDecoder = json => {
    open Json.Decode

    {token: json |> field("token", tokenDecoder)}
  }
}

module Encode = {
  let kindEncoder = x =>
    switch x {
    | #KFA1_2 => "fa1-2"
    | #KFA2 => "fa2"
    }

  let record = record => {
    open Json.Encode
    object_(list{
      ("kind", record.kind |> kindEncoder |> string),
      ("address", record.address |> PublicKeyHash.encoder),
    })
  }

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record)
}
