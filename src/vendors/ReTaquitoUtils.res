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
  | ValidateAPIError(int)

let () = Errors.registerHandler("Taquito", x =>
  switch x {
  | No_prefix_matched => I18n.Taquito.no_prefix_matched->Some
  | Invalid_checksum => I18n.Taquito.invalid_checksum->Some
  | Invalid_length => I18n.Taquito.invalid_length->Some
  | ValidateAPIError(n) => I18n.Taquito.api_error(n)->Some
  | _ => None
  }
)

@module("@taquito/utils")
external validateAddressRaw: string => int = "validateAddress"

@module("@taquito/utils")
external validateContractAddressRaw: string => int = "validateContractAddress"

let handleValidity = x =>
  switch x {
  | 0 => Error(No_prefix_matched)
  | 1 => Error(Invalid_checksum)
  | 2 => Error(Invalid_length)
  | 3 => Ok()
  | n => Error(ValidateAPIError(n))
  }

let validateAddress = s => s->validateAddressRaw->handleValidity->Result.map(() => #Address)

let validateContractAddress = s =>
  s->validateContractAddressRaw->handleValidity->Result.map(() => #Contract)

let validateAnyAddress = s =>
  s->validateContractAddress->Result.flatMapError(_ => s->validateAddress)

@module("@taquito/utils")
external bytes2Char: bytes => string = "bytes2Char"

type prefix = Js.TypedArray2.Uint8Array.t

type prefixes = {
  spsk: prefix,
  sppk: prefix,
  edpk: prefix,
  tz1: prefix,
  tz2: prefix,
  edsk: prefix,
}

@module("@taquito/utils") external prefix: prefixes = "prefix"

@module("@taquito/utils")
external b58cencode: (string, prefix) => string = "b58cencode"

@module("@taquito/utils")
external b58cdecode: (string, prefix) => Js.TypedArray2.Uint8Array.t = "b58cdecode"

@module("@taquito/utils")
external b58cencodeArray: (Js.TypedArray2.Uint8Array.t, prefix) => string = "b58cencode"

@module("@taquito/utils")
external encodeKeyHash: string => string = "encodeKeyHash"