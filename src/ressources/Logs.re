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
  | Connection
  | Account
  | Aliases
  | Delegate
  | Balance
  | Tokens
  | Settings
  | Beacon
  | Secret
  | Batch
  | Logs;

type kind =
  | Info
  | Warning
  | Error;

type timestamp = float;

type t = {
  kind,
  origin,
  errorScope: option(string),
  timestamp,
  msg: string,
};

let log = (~kind, ~origin, ~errorScope=?, msg) => {
  kind,
  timestamp: Js.Date.now(),
  errorScope,
  origin,
  msg,
};

let info = log(~kind=Info);
let error = (~origin, error) => {
  let Errors.{scope, msg} = error->Errors.getInfos;
  log(~kind=Error, ~origin, ~errorScope=scope, msg);
};

let originToString = e => {
  switch (e) {
  | Operation => "Operation"
  | Connection => "Connection"
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
  };
};
