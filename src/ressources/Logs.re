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

// Global errors description

type origin =
  | Operation
  | Account
  | Aliases
  | Delegate
  | Balance
  | Tokens
  | Settings
  | Update
  | Beacon
  | Secret
  | Batch
  | Nft
  | CustomAuth
  | Logs;

type kind =
  | Info
  | Warning
  | Error;

type timestamp = float;

type btn = {
  text: string,
  onPress: unit => unit,
};

type t = {
  kind,
  origin,
  errorScope: option(string),
  timestamp,
  msg: string,
  btns: option(list(btn)),
};

let log = (~kind, ~origin, ~errorScope=?, ~btns=?, msg) => {
  kind,
  timestamp: Js.Date.now(),
  errorScope,
  origin,
  msg,
  btns,
};

let info = log(~kind=Info);
let error = (~origin, error) => {
  let Errors.{scope, msg} = error->Errors.getInfos;
  log(~kind=Error, ~origin, ~errorScope=scope, msg);
};

let similarMessages = (m1, m2) => {
  let arr1 = Js.Array2.fromMap(Js.String2.castToArrayLike(m1), x => x);
  let arr2 = Js.Array2.fromMap(Js.String2.castToArrayLike(m2), x => x);

  let rec sameLetters = (acc, i) => {
    switch (arr1->Array.get(i), arr2->Array.get(i)) {
    | (Some(s1), Some(s2)) => sameLetters(s1 == s2 ? acc + 1 : acc, i + 1)
    | _ => acc
    };
  };

  let similarityRatio = 50. /. 100.;

  m1->Js.String.length == m2->Js.String.length
  && sameLetters(0, 0)->Float.fromInt
  /. arr1->Array.length->Float.fromInt > similarityRatio;
};

let similar = (l1, l2) => {
  l1.kind == l2.kind
  && l1.errorScope == l2.errorScope
  && l1.origin == l2.origin
  && similarMessages(l1.msg, l2.msg)
  && Float.abs(l1.timestamp -. l2.timestamp) < 2000.;
};

let originToString = e => {
  switch (e) {
  | Operation => "Operation"
  | Balance => "Balance"
  | Aliases => "Aliases"
  | Account => "Account"
  | Delegate => "Delegate"
  | Tokens => "Tokens"
  | Settings => "Settings"
  | Beacon => "Beacon"
  | Secret => "Secret"
  | Logs => "Logs"
  | Batch => "Batch"
  | Nft => "Nft"
  | CustomAuth => "CustomAuth"
  | Update => "Update"
  };
};
