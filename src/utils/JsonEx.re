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
    | ParsingError(s) => I18n.Errors.json_parsing_error(s)->Some
    | _ => None,
  );

external unsafeFromAny: 'a => Js.Json.t = "%identity";

[@bs.val] [@bs.scope "JSON"]
external stringify: Js.Json.t => string = "stringify";

[@bs.val] [@bs.scope "JSON"]
external stringifyAnyWithSpace:
  ('a, [@bs.as {json|null|json}] _, int) => string =
  "stringify";

/* bs-json uses exceptions instead of results, hence this function catches these
   exceptions to then build a result. */
let decode = (json, decoder) =>
  try(decoder(json)->Ok) {
  | Decode.DecodeError(s) => Error(DecodeError(s))
  | e =>
    Js.log(e);
    Error(Errors.Generic("Unknown decoding error"));
  };

let parse = s =>
  try(s->Js.Json.parseExn->Ok) {
  | _ => Error(ParsingError(s))
  };

let filterJsonExn = ex =>
  switch (ex) {
  | Json.ParseError(error) => error
  | Json.Decode.DecodeError(error) => error
  | _ => "Unknown error"
  };

let getExn =
  fun
  | Ok(v) => v
  | Error(e) => raise(Json.Decode.DecodeError(Errors.toString(e)));

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
