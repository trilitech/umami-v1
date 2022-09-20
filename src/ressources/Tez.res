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

module Infix = {
  open Int64

  let \"+" = add
  let \"-" = sub
  let \"*" = mul
  let \"/" = div
  let mod = rem
}

open Infix

type t = Int64.t // represented as mutez internally

let zero = Int64.zero

let oneTez = Int64.of_int(1000000)
let tezExp10 = 6

let toMutez = i => i * oneTez

let rec mulExp10 = (x, n) => {
  open Int64
  n <= 0
    ? x
    : mulExp10(
        x * of_int(10),
        {
          open Int
          n - 1
        },
      )
}

let int64OfString = s =>
  try Some(s->Int64.of_string) catch {
  | Failure(_) => None
  }

// `fromString` tries to read a string, returns None if
// it is not a formatted tez
let fromString = (tezStr): option<t> => {
  open Int64
  switch Js.String.split(".", tezStr) {
  | [v] => v->int64OfString->Option.map(toMutez)
  | [integer, floating] =>
    switch (int64OfString(integer), int64OfString(floating)) {
    | (Some(integer64), Some(floating64)) =>
      let floating64 = {
        let floatingMult = one->mulExp10({
          open Int
          tezExp10 - floating->Js.String.length
        })
        floating64 == zero ? zero : floating64 * floatingMult
      }
      Some(integer64->toMutez + floating64)
    | (_, _) => None
    }
  | _ => None
  }
}

let fromMutezInt = Int64.of_int
let unsafeToMutezInt = Int64.to_int
let fromMutezString = Int64.of_string
let toInt64 = x => x
let ofInt64 = x => x

let toBigNumber = x => x->toInt64->ReBigNumber.fromInt64

let toString = (tez: t) => {
  open Int64
  let integer = to_string(tez / oneTez)
  let floating = mod(tez, oneTez)
  let floatingStr = to_string(floating)
  let fLen = Js.String.length(floatingStr)

  let leading0 =
    fLen >= tezExp10 && floating != zero
      ? ""
      : Js.String.repeat(
          {
            open Int
            tezExp10 - fLen
          },
          "0",
        )
  let floatingStr = leading0 ++ floatingStr
  integer ++ ("." ++ floatingStr)
}

let formatString = s => s->fromString->Option.map(toString)

let decoder = json => json |> Json.Decode.string |> fromMutezString
