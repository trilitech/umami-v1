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

open TokenRepr;
module Unit = Unit;

type t = TokenRepr.t;

module Decode = {
  let kindFromString = tokenId =>
    fun
    | "fa1-2" => Ok(FA1_2)
    | "fa2" => Ok(FA2(tokenId))
    | k => Error(k);

  let tokenIdDecoder = json =>
    Json.Decode.(json |> optional(field("tokenId", int)))
    ->Option.getWithDefault(0);

  let kindDecoder = json => {
    let tokenId = tokenIdDecoder(json);

    Json.Decode.(json |> optional(field("kind", string)))
    ->Option.map(kindFromString(tokenId))
    ->Option.getWithDefault(Ok(FA1_2))
    ->Result.getExn;
  };

  let record = json =>
    Json.Decode.{
      kind: kindDecoder(json),
      address:
        json
        |> field("address", string)
        |> PublicKeyHash.build
        |> Result.getExn,
      alias: json |> field("alias", string),
      symbol: json |> field("symbol", string),
      chain:
        (json |> field("chain", optional(string)))
        ->Option.getWithDefault(Network.granadanetChain),
      decimals:
        (json |> optional(field("decimals", int)))
        ->Option.getWithDefault(0),
    };

  let array = json => json |> Json.Decode.array(record);

  let viewer = Json.Decode.string;
};

module Encode = {
  let kindEncoding =
    fun
    | FA1_2 => ("fa1-2", 0)
    | FA2(i) => ("fa2", i);

  let record = record => {
    let (kind, tokenId) = kindEncoding(record.kind);
    Json.Encode.(
      object_([
        ("kind", kind |> string),
        ("address", (record.address :> string) |> string),
        ("alias", record.alias |> string),
        ("symbol", record.symbol |> string),
        ("chain", record.chain |> string),
        ("decimals", record.decimals |> int),
        ("tokenId", tokenId |> int),
      ])
    );
  };

  let array = arrayRecord => arrayRecord |> Json.Encode.array(record);
};
