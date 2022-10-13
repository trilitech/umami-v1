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
module Unit = Unit

type t = TokenRepr.t

let defaultSymbol = (address: PublicKeyHash.t) =>
  (address :> string)->Js.String2.substrAtMost(~from=0, ~length=6)

let defaultName = (address: PublicKeyHash.t, tokenId) =>
  defaultSymbol(address) ++ ("#" ++ tokenId->Int.toString)

module Decode = {
  open Json.Decode

  let kindFromString = (tokenId, x) =>
    switch x {
    | "fa1-2" => Ok(FA1_2)
    | "fa2" => Ok(FA2(tokenId))
    | k => Error(k)
    }

  let tokenIdDecoder = json => (json |> optional(field("tokenId", int)))->Option.getWithDefault(0)

  let kindDecoder = json => {
    let tokenId = tokenIdDecoder(json)

    (json |> optional(field("kind", string)))
    ->Option.map(kindFromString(tokenId))
    ->Option.getWithDefault(Ok(FA1_2))
    ->Result.getExn
  }

  // Some NFTs don't have symbol, illformed metadata but real examples unfortunately
  let symbolDecoder = (default, json) =>
    (json |> optional(field("symbol", string)))->Option.getWithDefault(default)

  // Name is not mandatory in TZIP-12... This should not happen, and is a way to
  // recover from broken cache
  let aliasDecoder = (default, json) =>
    (json |> optional(field("alias", string)))->Option.getWithDefault(default)

  let record = json => {
    let address = json |> field("address", string) |> PublicKeyHash.build |> Result.getExn
    let kind = kindDecoder(json)
    let defaultName = defaultName(address, kind->TokenRepr.kindId)
    let defaultSymbol = defaultSymbol(address)
    {
      kind: kind,
      address: address,
      alias: json |> aliasDecoder(defaultName),
      symbol: json |> symbolDecoder(defaultSymbol),
      chain: (json |> field("chain", optional(Network.Decode.chainIdDecoder)))
        ->Option.getWithDefault(Network.getChainId(#Granadanet)),
      decimals: (json |> optional(field("decimals", int)))->Option.getWithDefault(0),
      asset: (json |> optional(field("asset", Metadata.Decode.assetDecoder)))
        ->Option.getWithDefault(Metadata.defaultAsset),
    }
  }

  let array = json => json |> array(record)

  let viewer = string
}

module Encode = {
  open Json.Encode
  let kindEncoding = x =>
    switch x {
    | FA1_2 => ("fa1-2", 0)
    | FA2(i) => ("fa2", i)
    }

  let record = record => {
    let (kind, tokenId) = kindEncoding(record.kind)
    object_(list{
      ("kind", kind |> string),
      ("address", (record.address :> string) |> string),
      ("alias", record.alias |> string),
      ("symbol", record.symbol |> string),
      ("chain", record.chain |> Network.Encode.chainIdEncoder),
      ("decimals", record.decimals |> int),
      ("tokenId", tokenId |> int),
      ("asset", record.asset |> Metadata.Encode.assetEncoder),
    })
  }

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record)
}
