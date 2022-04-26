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

type token = {
  amount: TokenRepr.Unit.t,
  token: TokenRepr.t,
};

type t =
  | Tez(Tez.t)
  | Token(token);

let makeTez = t => t->Tez;
let makeToken = (~amount, ~token) => Token({amount, token});

let toString =
  fun
  | Tez(tez) => tez->Tez.toString
  | Token({amount, token}) =>
    amount->TokenRepr.Unit.toStringDecimals(token.decimals);

let isToken =
  fun
  | Tez(_) => false
  | Token(_) => true;

let getTez =
  fun
  | Tez(tez) => Some(tez)
  | _ => None;

let getToken =
  fun
  | Token(a) => Some(a)
  | _ => None;

let show =
  fun
  | Tez(v) => I18n.tez_amount(v->Tez.toString)
  | Token({amount, token}) =>
    I18n.amount(
      amount->TokenRepr.Unit.toStringDecimals(token.decimals),
      token.symbol,
    );

let compareCurrencies = (v1, v2) => {
  switch (v1, v2) {
  | (Tez(_), Token(_)) => (-1)
  | (Token(_), Tez(_)) => 1
  | _ => 0
  };
};

let reduce = l =>
  l
  ->List.reduceGroupBy(
      ~group=
        fun
        | Tez(_) => None
        | Token({token}) => Some(token),
      ~map=(acc, v) =>
      switch (acc, v) {
      | (None, v) => v
      | (Some(Tez(acc)), Tez(v)) => Tez(Tez.Infix.(acc + v))
      | (Some(Token({amount: acc, token})), Token({amount})) =>
        Token({amount: TokenRepr.Unit.Infix.(acc + amount), token})
      | (Some(acc), _) => acc
      }
    )
  ->List.map(snd)
  ->List.sort(compareCurrencies);