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

type t = string

type Errors.t +=
  | NotAnImplicit(string)
  | InvalidScheme(string)
  | NotAContract(string)

let () = Errors.registerHandler("PublicKeyHash", x =>
  switch x {
  | InvalidScheme(s) => I18n.Wallet.invalid_scheme(s)->Some
  | NotAnImplicit(a) => I18n.Csv.cannot_parse_address(a, I18n.Taquito.not_an_account)->Some
  | NotAContract(a) => I18n.Csv.cannot_parse_address(a, I18n.Taquito.not_a_contract)->Some
  | _ => None
  }
)

type parsedAddress =
  | Contract(t)
  | Implicit(t)

let build = s =>
  switch ReTaquitoUtils.validateAddress(s) {
  | Ok(#Address) => Ok(s)
  | Error(e) => Error(e)
  }

let isContract = (k: t) =>
  switch ReTaquitoUtils.validateAnyAddress(k) {
  | Ok(#Contract) => true
  | Ok(#Address) => false
  | Error(_) => false
  //no need to handle the error case. k is already a pkh
  }

let isImplicit = (k: t) =>
  switch ReTaquitoUtils.validateAnyAddress(k) {
  | Ok(#Contract) => false
  | Ok(#Address) => true
  | Error(_) => false
  //no need to handle the error case. k is already a pkh
  }

let buildAny = s =>
  switch ReTaquitoUtils.validateAnyAddress(s) {
  | Ok(#Contract) => Ok(Contract(s))
  | Ok(#Address) => Ok(Implicit(s))
  | Error(e) => Error(e)
  }

let buildContract = (a: string) =>
  switch buildAny(a) {
  | Ok(Contract(a)) => Ok(a)
  | Ok(Implicit(a)) => Error(NotAContract(a))
  | Error(e) => Error(e)
  }

let buildImplicit = (a: string) =>
  switch buildAny(a) {
  | Ok(Implicit(a)) => Ok(a)
  | Ok(Contract(a)) => Error(NotAnImplicit(a))
  | Error(e) => Error(e)
  }

let getShrinked = (address: t, ~n=6, ()) => {
  let address = (address :> string)
  let startSlice = address->Js.String2.slice(~from=0, ~to_=n - 1)
  let endSlice = address->Js.String2.sliceToEnd(~from=-n - 1)
  `${startSlice}...${endSlice}`
}

module Scheme = {
  type t =
    | ED25519
    | SECP256K1
    | P256

  let toString = x =>
    switch x {
    | ED25519 => "ed25519"
    | SECP256K1 => "secp256k1"
    | P256 => "P-256"
    }

  let fromString = x =>
    switch x {
    | "ed25519" => Ok(ED25519)
    | "secp256k1" => Ok(SECP256K1)
    | "P-256" => Ok(P256)
    | s => Error(InvalidScheme(s))
    }
}

type implicit =
  | TZ1
  | TZ2
  | TZ3

type kind =
  | Implicit(implicit)
  | KT1

let implicitFromScheme = x =>
  switch x {
  | Scheme.ED25519 => TZ1
  | SECP256K1 => TZ2
  | P256 => TZ3
  }

let kindToString = x =>
  switch x {
  | Implicit(TZ1) => "tz1"
  | Implicit(TZ2) => "tz2"
  | Implicit(TZ3) => "tz3"
  | KT1 => "kt1"
  }

module DerivationPath = {
  let default = "m/44'/1729'/?'/0'"
}

type pkh = t

module Comparator = Belt.Id.MakeComparable({
  type t = pkh
  let cmp = compare
})

module Map = Map.Make(Comparator)

let encoder = address => Json.Encode.string(address)
let decoder = json => json |> Json.Decode.string |> build |> JsonEx.getExn

let unsafeBuild = s => s
