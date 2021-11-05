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

open ProtocolOptions;

module Currency = {
  type t =
    | Tez(Tez.t)
    | Token(TokenRepr.Unit.t, TokenRepr.t);

  let makeTez = t => t->Tez;
  let makeToken = (~amount, ~token) => Token(amount, token);

  let toInt64 =
    fun
    | Tez(tez) => tez->Tez.toInt64
    | Token(curr, _) => curr->TokenRepr.Unit.toBigNumber->ReBigNumber.toInt64;

  let toBigNumber =
    fun
    | Tez(tez) => tez->Tez.toInt64->ReBigNumber.fromInt64
    | Token(curr, _) => curr->TokenRepr.Unit.toBigNumber;

  let toString =
    fun
    | Tez(tez) => tez->Tez.toString
    | Token(curr, _) => curr->TokenRepr.Unit.toNatString;

  let getTez =
    fun
    | Tez(tez) => Some(tez)
    | _ => None;

  let getToken =
    fun
    | Token(t, c) => Some((t, c))
    | _ => None;

  let getTokenExn = t => t->getToken->Option.getExn;

  let showAmount =
    fun
    | Tez(v) => I18n.t#tez_amount(v->Tez.toString)
    | Token(v, t) =>
      I18n.t#amount(
        v->TokenRepr.Unit.toStringDecimals(t.decimals),
        t.symbol,
      );

  let compareCurrencies = (v1, v2) => {
    switch (v1, v2) {
    | (Tez(_), Token(_)) => (-1)
    | (Token(_), Tez(_)) => 1
    | _ => 0
    };
  };

  let reduceAmounts = l =>
    l
    ->List.reduceGroupBy(
        ~group=
          fun
          | Tez(_) => None
          | Token(_, t) => Some(t),
        ~map=(acc, v) =>
        switch (acc, v) {
        | (None, v) => v
        | (Some(Tez(acc)), Tez(v)) => Tez(Tez.Infix.(acc + v))
        | (Some(Token(acc, t)), Token(v, _)) =>
          Token(TokenRepr.Unit.Infix.(acc + v), t)
        | (Some(acc), _) => acc
        }
      )
    ->List.map(snd)
    ->List.sort(compareCurrencies);
};

type elt = {
  destination: PublicKeyHash.t,
  amount: Currency.t,
  tx_options: transferEltOptions,
};

type t = {
  source: Account.t,
  transfers: list(elt),
  options: transferOptions,
};

let makeSingleTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) => {
  destination,
  amount,
  tx_options:
    makeTransferEltOptions(
      ~fee?,
      ~gasLimit?,
      ~storageLimit?,
      ~parameter?,
      ~entrypoint?,
      (),
    ),
};

let makeSingleTezTransferElt =
    (
      ~destination,
      ~amount,
      ~fee=?,
      ~parameter=?,
      ~entrypoint=?,
      ~gasLimit=?,
      ~storageLimit=?,
      (),
    ) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=Currency.makeTez(amount),
    ~fee?,
    ~parameter?,
    ~entrypoint?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

/* Tokens cannot define parameter and entrypoint, since they are
   already translated as parameters into an entrypoint */
let makeSingleTokenTransferElt =
    (~destination, ~amount, ~token, ~fee=?, ~gasLimit=?, ~storageLimit=?, ()) =>
  makeSingleTransferElt(
    ~destination,
    ~amount=Currency.makeToken(~amount, ~token),
    ~fee?,
    ~gasLimit?,
    ~storageLimit?,
    (),
  );

let makeTransfers = (~source, ~transfers, ~burnCap=?, ~forceLowFee=?, ()) => {
  let options = makeTransferOptions(~burnCap?, ~forceLowFee?, ());
  {source, transfers, options};
};
