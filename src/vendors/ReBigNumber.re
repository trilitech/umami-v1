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

type t;

[@bs.module] [@bs.new] external fromString: string => t = "bignumber.js";
[@bs.module] [@bs.new] external fromInt: int => t = "bignumber.js";

[@bs.send] external toInt: t => int = "toNumber";
[@bs.send] external toFloat: t => float = "toNumber";
[@bs.send] external toString: t => string = "toString";
[@bs.send] external toFixed: (t, option(int)) => string = "toFixed";
[@bs.send] external plus: (t, t) => t = "plus";
[@bs.send] external isNaN: t => bool = "isNaN";
[@bs.send] external integerValue: t => t = "integerValue";
[@bs.send] external isInteger: t => bool = "isInteger";
[@bs.send] external isNegative: t => bool = "isNegative";
[@bs.send] external div: (t, t) => t = "div";
[@bs.send] external times: (t, t) => t = "times";
[@bs.send] external powInt: (t, int) => t = "pow";
[@bs.send] external isEqualTo: (t, t) => bool = "isEqualTo";

let toFixed = (~decimals=?, t) => toFixed(t, decimals);

let fromInt64 = i => i->Int64.to_string->fromString;
let toInt64 = i => i->toFixed->Int64.of_string;
