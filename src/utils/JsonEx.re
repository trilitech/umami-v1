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

open Json;

type Errors.t +=
  | ParsingError(string)
  | DecodeError(string);

let () =
  Errors.registerHandler(
    "Json",
    fun
    | DecodeError(s) => Some(s)
    | ParsingError(s) => I18n.errors#json_parsing_error(s)->Some
    | _ => None,
  );

/* Propagates Errors.t during decoding */
exception InternalError(Errors.t);

let internalError = e => InternalError(e);

/* bs-json uses exceptions instead of results, hence this function catches these
   exceptions to then build a result. */
let decode = (json, decoder) =>
  try(decoder(json)->Ok) {
  | Decode.DecodeError(s) => Error(DecodeError(s))
  | InternalError(e) => Error(e)
  | e =>
    Js.log(e);
    Error(Errors.Generic("Unknown decoding error"));
  };

let parse = s =>
  try(s->Js.Json.parseExn->Ok) {
  | _ => Error(ParsingError(s))
  };

module MichelsonDecode = {
  open Decode;

  type address =
    | Packed(bytes)
    | Pkh(PublicKeyHash.t);

  let dataDecoder = params => field("data", array(params));

  let pairDecoder = (d1, d2) => field("args", tuple2(d1, d2));

  let intDecoder = field("int", string);

  let bytesDecoder =
    field("bytes", string) |> map(s => s->Bytes.unsafe_of_string);

  let stringDecoder = field("string", string);

  let addressDecoder =
    either(
      bytesDecoder |> map(b => Packed(b)),
      stringDecoder
      |> map(s =>
           Pkh(s->PublicKeyHash.build->Result.getWithExn(internalError))
         ),
    );

  /*
    Example of response for `run_view` on `balance_of` on an FA2 contract for
    a single address:
    { "data":
        [ { "prim": "Pair",
            "args":
              [ { "prim": "Pair",
                  "args":
                    [ { "bytes": "0000721765c758aacce0986e781ddc9a40f5b6b9d9c3" },
                      { "int": "0" } ] }, { "int": "1000010000" } ] } ] }

    The result is actually a Michelson list of `(pkh * tokenId * balance)`,
   but this version only parses the result for a single address.
   This version is purely adhoc for this response.
   */
  let fa2BalanceOfDecoder = json =>
    json
    |> dataDecoder(
         pairDecoder(pairDecoder(addressDecoder, intDecoder), intDecoder),
       );
};

module Encode = {
  include Json.Encode;

  let rec bsListEncoder = (valueEncoder, l) =>
    switch (l) {
    | [] => int(0)
    | [hd, ...tl] =>
      object_([
        ("hd", valueEncoder(hd)),
        ("tl", bsListEncoder(valueEncoder, tl)),
      ])
    };
};

module Decode = {
  include Json.Decode;

  let optionalOrNull = (fieldName, decoder, json) =>
    (json |> optional(field(fieldName, optional(decoder))))
    ->Option.getWithDefault(None);

  let nilDecoder = json =>
    json->int == 0 ? [] : raise(DecodeError("invalid int for nil"));

  let rec consDecoder = (valueDecoder, json) => {
    let v = json |> field("hd", valueDecoder);
    let tl = json |> field("tl", bsListDecoder(valueDecoder));
    [v, ...tl];
  }

  and bsListDecoder = (valueDecoder, json) =>
    json |> either(consDecoder(valueDecoder), nilDecoder);
};
