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

type Errors.t +=
  | ParsingError(string)
  | DecodeError(string)

let parse: string => Promise.result<Js.Json.t>

let decode: (Js.Json.t, Json.Decode.decoder<'a>) => Promise.result<'a>

let stringify: Js.Json.t => string

let unsafeFromAny: 'a => Js.Json.t

let stringifyAnyWithSpace: ('a, int) => string

let filterJsonExn: exn => string

// Raises DecodeError if first parameter is Error
let getExn: Promise.result<'a> => 'a

module Encode: {
  include module type of Json.Encode

  let bsListEncoder: encoder<'a> => encoder<list<'a>>
}

module Decode: {
  include module type of Json.Decode

  let optionalOrNull: (string, decoder<'a>) => decoder<option<'a>>

  let bsListDecoder: decoder<'a> => decoder<list<'a>>
}
