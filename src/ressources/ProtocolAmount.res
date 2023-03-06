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

type token = {
  source: PublicKeyHash.t, //FIXME: move this elsewhere?
  amount: TokenRepr.Unit.t,
  token: TokenRepr.t,
}

type t =
  | Tez(Tez.t)
  | Token(token)

let makeTez = t => t->Tez
let makeToken = (~source, ~amount, ~token) => Token({source: source, amount: amount, token: token})

let isZero = x =>
  switch x {
  | Tez(t) => Tez.zero == t
  | Token({amount: a}) => TokenRepr.Unit.zero == a
  }

let toString = x =>
  switch x {
  | Tez(tez) => tez->Tez.toString
  | Token({amount, token}) => amount->TokenRepr.Unit.toStringDecimals(token.decimals)
  }

let isToken = x =>
  switch x {
  | Tez(_) => false
  | Token(_) => true
  }

let getTez = x =>
  switch x {
  | Tez(tez) => Some(tez)
  | _ => None
  }

let getToken = x =>
  switch x {
  | Token(a) => Some(a)
  | _ => None
  }

let show = x =>
  switch x {
  | Tez(v) => I18n.tez_amount(v->Tez.toString)
  | Token({amount, token}) =>
    I18n.amount(amount->TokenRepr.Unit.toStringDecimals(token.decimals), token.symbol)
  }

let compareCurrencies = (v1, v2) =>
  switch (v1, v2) {
  | (Tez(_), Token(_)) => -1
  | (Token(_), Tez(_)) => 1
  | _ => 0
  }

let reduce = l =>
  l
  ->List.reduceGroupBy(
    ~group=x =>
      switch x {
      | Tez(_) => None
      | Token({token}) => Some(token)
      },
    ~map=(acc, v) =>
      switch (acc, v) {
      | (None, v) => v
      | (Some(Tez(acc)), Tez(v)) =>
        Tez({
          open Tez.Infix
          acc + v
        })
      | (Some(Token({amount: acc, token, source})), Token({amount})) =>
        Token({
          amount: {
            open TokenRepr.Unit.Infix
            acc + amount
          },
          token: token,
          source: source,
        })
      | (Some(acc), _) => acc
      },
  )
  ->List.map(snd)
  ->List.sort(compareCurrencies)
