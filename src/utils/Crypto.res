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

module Uint8Array = Js.TypedArray2.Uint8Array

module Elliptic = {
  type t
  type bn
  type ecKind = [#secp256k1]

  type simplePoint = {
    x: string,
    y: string,
  }

  type point = {
    x: bn,
    y: bn,
  }

  type keyPair

  @module("elliptic") @new external make: ecKind => t = "ec"

  @send
  external keyFromPublic: (t, simplePoint) => keyPair = "keyFromPublic"

  @send external getPublic: keyPair => point = "getPublic"
  @send external getY: point => bn = "getY"
  @send external getX: point => bn = "getX"
  @send external toArray: bn => array<int> = "toArray"
}

@module("blakejs")
external blake2b: (Uint8Array.t, option<Uint8Array.t>, int) => string = "blake2b"

let pk2pkh = pk =>
  if pk->Js.String.length == 54 && pk->Js.String.slice(~from=0, ~to_=4) == "edpk" {
    let pkDecoded = {
      open ReTaquitoUtils
      b58cdecode(pk, prefix.edpk)
    }
    {
      open ReTaquitoUtils
      b58cencode(blake2b(pkDecoded, None, 20), prefix.tz1)
    }->Ok
  } else if pk->Js.String.length == 55 && pk->Js.String.slice(~from=0, ~to_=4) == "sppk" {
    let pkDecoded = {
      open ReTaquitoUtils
      b58cdecode(pk, prefix.sppk)
    }
    {
      open ReTaquitoUtils
      b58cencode(blake2b(pkDecoded, None, 20), prefix.tz2)
    }->Ok
  } else {
    Error(Errors.Generic("Invalid public key"))
  }

let spPointsToPkh = (~x, ~y) => {
  open Elliptic

  let key = Elliptic.make(#secp256k1)->keyFromPublic({x: x, y: y})
  let yArray = key->getPublic->getY->toArray
  let prefixVal = mod(yArray->Js.Array2.unsafe_get(yArray->Array.length - 1), 2) != 0 ? 3 : 2
  let pad = Array.make(32, 0)

  let publicKey = Js.TypedArray2.Uint8Array.make(
    [prefixVal]->Js.Array2.concat(
      pad->Js.Array2.concat(key->getPublic->getX->toArray)->Js.Array2.sliceFrom(-32),
    ),
  )

  let pk = {
    open ReTaquitoUtils
    b58cencodeArray(publicKey, prefix.sppk)
  }

  pk2pkh(pk)->Result.flatMap(PublicKeyHash.build)
}
