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

type raw = {message: string};

let toRaw: Js.Promise.error => raw = Obj.magic;

let branchRefused = "branch refused";
let wrongSecretKey = "wrong secret key";
let badPkh = "Unexpected data (Signature.Public_key_hash)";
let unregisteredDelegate = "contract.manager.unregistered_delegate";
let unchangedDelegate = "contract.manager.delegate.unchanged";
let invalidContract = "Invalid contract notation";
let emptyTransaction = "contract.empty_transaction";

type t =
  | Generic(string)
  | WrongPassword
  | UnregisteredDelegate
  | UnchangedDelegate
  | EmptyTransaction
  | InvalidContract
  | BranchRefused
  | LedgerInit(string)
  | LedgerNotReady
  | LedgerMasterKeyRetrieval(string)
  | BadPkh;

let parse = e =>
  switch (e.message) {
  | s when s->Js.String2.includes(wrongSecretKey) => WrongPassword
  | s when s->Js.String2.includes(branchRefused) => BranchRefused
  | s when s->Js.String2.includes(badPkh) => BadPkh
  | s when s->Js.String2.includes(unregisteredDelegate) =>
    UnregisteredDelegate
  | s when s->Js.String2.includes(unchangedDelegate) => UnchangedDelegate
  | s when s->Js.String2.includes(invalidContract) => InvalidContract
  | s when s->Js.String2.includes(emptyTransaction) => EmptyTransaction
  | s => Generic(Js.String.make(s))
  };

let fromPromiseParsedWrapper = (wrapError, p) =>
  p->FutureJs.fromPromise(e => {
    let e = e->toRaw;
    Js.log(e.message);

    e->parse->wrapError;
  });

let fromPromiseParsed = res => fromPromiseParsedWrapper(x => x, res);
