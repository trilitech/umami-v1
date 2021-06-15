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

module Unit = {
  type t = ReBigNumber.t;
  open ReBigNumber;

  let toBigNumber = x => x;
  let fromBigNumber = x =>
    x->isNaN || !x->isInteger || x->isNegative ? None : x->Some;

  let toNatString = toFixed;
  let fromNatString = s => s->fromString->fromBigNumber;
  let forceFromString = s => {
    let v = s->fromString;
    v->isNaN ? None : v->isInteger ? v->integerValue->Some : None;
  };

  let isValid = v =>
    v->fromNatString->Option.mapWithDefault(false, isInteger);

  let zero = fromString("0");

  let add = plus;

  module Infix = {
    let (+) = plus;
  };
};

type address = string;

type t = {
  address,
  alias: string,
  symbol: string,
  chain: string,
};