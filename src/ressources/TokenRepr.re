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

  type illformed =
    | NaN
    | Float
    | Negative;

  open ReBigNumber;
  let toBigNumber = x => x;
  let fromBigNumber = x =>
    if (x->isNaN) {
      Error(NaN);
    } else if (!x->isInteger) {
      Error(Float);
    } else if (x->isNegative) {
      Error(Negative);
    } else {
      x->Ok;
    };

  let isNat = v => v->isInteger && !v->isNegative && !v->isNaN;

  let toNatString = toFixed;
  let toStringDecimals = (x, decimals) =>
    if (decimals == 0) {
      x->toNatString;
    } else {
      let shift = fromInt(10)->powInt(decimals);
      x->div(shift)->toNatString;
    };
  let fromNatString = s => s->fromString->fromBigNumber;
  let fromStringDecimals = (s, decimals) => {
    let shift = fromInt(10)->powInt(decimals);
    let x = s->fromString->times(shift);
    x->fromBigNumber;
  };

  let formatString = (s, decimals) => {
    let x = fromStringDecimals(s, decimals);
    x->Result.map(x => toStringDecimals(x, decimals));
  };

  let forceFromString = s => {
    let v = s->fromString;
    v->isNaN ? None : v->isInteger ? v->integerValue->Some : None;
  };

  let zero = fromString("0");

  let add = plus;

  module Infix = {
    let (+) = plus;
  };
};

type address = PublicKeyHash.t;

type kind =
  | FA1_2
  | FA2(int);

type t = {
  kind,
  address,
  alias: string,
  symbol: string,
  chain: string,
  decimals: int,
};
