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
    | Float(option(int))
    | Negative;

  type Errors.t +=
    | IllformedTokenUnit(ReBigNumber.t, illformed);

  let () =
    Errors.registerHandler(
      "TokenRepr.Unit",
      fun
      | IllformedTokenUnit(e, NaN) =>
        I18n.Errors.not_a_number(ReBigNumber.toString(e))->Some
      | IllformedTokenUnit(e, Negative) =>
        I18n.Errors.negative_number(ReBigNumber.toString(e))->Some
      | IllformedTokenUnit(e, Float(None | Some(0))) =>
        I18n.Errors.unexpected_decimals(ReBigNumber.toString(e))->Some
      | IllformedTokenUnit(e, Float(Some(decimals))) =>
        I18n.Errors.expected_decimals(ReBigNumber.toString(e), decimals)
        ->Some
      | _ => None,
    );

  open ReBigNumber;
  let toBigNumber = x => x;
  let fromBigNumber = (x, allowFloat) =>
    if (x->isNaN) {
      Error(IllformedTokenUnit(x, NaN));
    } else if (!x->isInteger && !allowFloat) {
      Error(IllformedTokenUnit(x, Float(None)));
    } else if (x->isNegative) {
      Error(IllformedTokenUnit(x, Negative));
    } else {
      x->Ok;
    };

  let fromFloatBigNumber = (x, decimals) => {
    x
    ->fromBigNumber(true)
    ->Result.flatMap(v => {
        let shift = fromInt(10)->powInt(decimals);
        let x = v->times(shift);
        x->isInteger
          ? Ok(x) : Error(IllformedTokenUnit(v, Float(Some(decimals))));
      });
  };
  let fromBigNumber = x => x->fromBigNumber(false);

  let isNat = v => v->isInteger && !v->isNegative && !v->isNaN;

  let toNatString = toFixed;
  let toStringDecimals = (x, decimals) => {
    let shift = fromInt(10)->powInt(decimals);
    let x = x->div(shift);
    toFixed(~decimals, x);
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

  let one = fromString("1");
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
  chain: Network.chainId,
  decimals: int,
  asset: Metadata.asset,
};

let kindId =
  fun
  | FA1_2 => 0
  | FA2(n) => n;

// let idKind =
//   fun
//   | 0 => FA1_2
//   | n => FA2(n);

let id = ({kind}) => kind->kindId;

let toFlatJson = t => {
  let json = JsonEx.unsafeFromAny(t.asset);
  let dict = Js.Json.decodeObject(json);

  dict->Option.map(d => {
    let entries = d->Js.Dict.entries;
    let rootEntries = [|
      ("name", Js.Json.string(t.alias)),
      ("address", (t.address :> string)->Js.Json.string),
      ("token_id", t->id->float_of_int->Js.Json.number),
      ("symbol", Js.Json.string(t.symbol)),
      ("decimals", t.decimals->float_of_int->Js.Json.number),
    |];
    let entries = Array.concat(rootEntries, entries);
    entries->Js.Dict.fromArray->Js.Json.object_;
  });
};

let isNFT = t =>
  t.asset.Metadata.artifactUri != None
  || t.asset.Metadata.displayUri != None
  || t.asset.Metadata.isBooleanAmount == true;

let isFa2 = t =>
  switch (t.kind) {
  | FA1_2 => false
  | FA2(_) => true
  };

let decoder = json =>
  json
  |> Json.Decode.string
  |> ReBigNumber.fromString
  |> Unit.fromBigNumber
  |> (
    fun
    | Ok(v) => v
    | Error(e) => raise(Json.Decode.DecodeError(Errors.toString(e)))
  );

type token = (PublicKeyHash.t, option(int));

module Comparator =
  Belt.Id.MakeComparable({
    type t = (PublicKeyHash.t, token);
    let cmp = compare;
  });

module Map = Map.Make(Comparator);
