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

type error =
  | ParsingFailed(string)
  | MoreThan1Wildcard
  | MissingWildcardOr0
  | NotTezosBip44;

exception IllFormedPath;

let handleError =
  fun
  | ParsingFailed(_) => I18n.form_input_error#dp_not_a_dp
  | MoreThan1Wildcard => I18n.form_input_error#dp_more_than_1_wildcard
  | MissingWildcardOr0 => I18n.form_input_error#dp_missing_wildcard
  | NotTezosBip44 => I18n.form_input_error#dp_not_tezos;

type item =
  | Wildcard
  | Int(int);

type t = array(int);

type derivationPath = t;

let toString = impl =>
  impl->Array.map(i => Format.sprintf("/%d'", i))->Js.String.concatMany("m");

module Pattern = {
  type t = array(item);

  /* The only thing that can vary in a tezos bip44 derivation
     path is the address alias that is the last integer */
  type tezosBip44 = int;

  let default: tezosBip44 = 0;
  let defaultString = "m/44'/1729'/?'/0'";
  let fromTezosBip44 = n => [|Int(44), Int(1729), Wildcard, Int(n)|];
  let isDefault = dp => dp == default->fromTezosBip44;

  let convertToTezosBip44 =
    fun
    | [|Int(44), Int(1729), Wildcard, Int(n)|] => Ok(n)
    | _ => Error(NotTezosBip44);

  let toString = dp =>
    dp
    ->Array.map(
        fun
        | Wildcard => "/?'"
        | Int(i) => Format.sprintf("/%d'", i),
      )
    ->Js.String.concatMany("m");

  let implement = (dp, index) => {
    let replwc = v =>
      v->Array.map(
        fun
        | Wildcard => index
        | Int(i) => i,
      );
    if (dp->Array.getBy((==)(Wildcard)) != None) {
      dp->replwc;
    } else {
      let a = dp->replwc;
      let id = a->Js.Array2.indexOf(0);
      id == (-1) ? raise(IllFormedPath) : a->Array.set(id, index)->(_ => a);
    };
  };

  let regPath = [%re "/^m(\\/(\\d+|\\?)')+$/g"];
  let regItem = [%re "/\\d+|\\?/g"];

  let checkWildcard = dp => {
    Array.(dp->keep((==)(Wildcard))->length) <= 1
      ? Array.(dp->keep((==)(Int(0)))->length) <= 1
          ? Ok(dp) : Error(MissingWildcardOr0)
      : Error(MoreThan1Wildcard);
  };

  let fromString = s => {
    switch (Js.String.match(regPath, s)) {
    | Some(_) =>
      Js.String.match(regItem, s)
      ->Option.getExn // We already matched this pattern above
      ->Array.map(
          fun
          | "?" => Wildcard
          | i =>
            // We already matched the int above
            Int(i->Int.fromString->Option.getExn),
        )
      ->checkWildcard
    | None => Error(ParsingFailed(s))
    };
  };
};
