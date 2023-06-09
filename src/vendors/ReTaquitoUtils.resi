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
  | No_prefix_matched
  | Invalid_checksum
  | Invalid_length

let validateAddress: string => result<[> #Address], Errors.t>

let validateAnyAddress: string => result<[> #Address | #Contract], Errors.t>

let bytes2Char: bytes => string

type prefix = Js.TypedArray2.Uint8Array.t
type prefixes = {
  spsk: prefix,
  sppk: prefix,
  edpk: prefix,
  tz1: prefix,
  tz2: prefix,
  edsk: prefix,
}
let prefix: prefixes
let b58cencode: (string, prefix) => string
let b58cdecode: (string, prefix) => Js.TypedArray2.Uint8Array.t
let b58cencodeArray: (Js.TypedArray2.Uint8Array.t, prefix) => string
let encodeKeyHash: string => string
let b58decode: string => string
let encodePubKey: string => string
let hex2buf: string => Js.TypedArray2.Uint8Array.t
