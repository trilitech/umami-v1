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

type t

@module @new external fromString: string => t = "bignumber.js"
@module @new external fromInt: int => t = "bignumber.js"

@send external toInt: t => int = "toNumber"
@send external toFloat: t => float = "toNumber"
@send external toString: t => string = "toString"
@send external toFixed: (t, option<int>) => string = "toFixed"
@send external plus: (t, t) => t = "plus"
@send external isNaN: t => bool = "isNaN"
@send external integerValue: t => t = "integerValue"
@send external isInteger: t => bool = "isInteger"
@send external isNegative: t => bool = "isNegative"
@send external div: (t, t) => t = "div"
@send external times: (t, t) => t = "times"
@send external powInt: (t, int) => t = "pow"
@send external isEqualTo: (t, t) => bool = "isEqualTo"
@send external isLessThanOrEqualTo: (t, t) => bool = "isLessThanOrEqualTo"
@send external isGreaterThanOrEqualTo: (t, t) => bool = "isGreaterThanOrEqualTo"
@send external comparedTo: (t, t) => int = "comparedTo"
@send external minus: (t, t) => t = "minus"

let toFixed = (~decimals=?, t) => toFixed(t, decimals)

let fromInt64 = i => i->Int64.to_string->fromString
let toInt64 = i => i->toFixed->Int64.of_string

let zero = fromInt(0)

type nat = t

module Comparator = Belt.Id.MakeComparable({
  type t = nat
  let cmp = comparedTo
})

module Map = Map.Make(Comparator)