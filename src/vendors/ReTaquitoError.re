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

let branchRefused = "branch refused";
let wrongSecretKey = "wrong secret key";
let badPkh = "Unexpected data (Signature.Public_key_hash)";
let unregisteredDelegate = "contract.manager.unregistered_delegate";
let unchangedDelegate = "contract.manager.delegate.unchanged";
let invalidContract = "Invalid contract notation";
let emptyTransaction = "contract.empty_transaction";
let ledgerTimeout = "No Ledger device found (timeout)";
let ledgerKey = "Unable to retrieve public key";
let ledgerDenied = "0x6985";

type Errors.t +=
  | WrongPassword
  | UnregisteredDelegate
  | UnchangedDelegate
  | EmptyTransaction
  | InvalidContract
  | BranchRefused
  | BadPkh
  | LedgerInitTimeout
  | LedgerInit(string)
  | LedgerNotReady
  | LedgerKeyRetrieval
  | LedgerDenied
  | SignerIntentInconsistency;

let parse = (e: RawJsError.t) =>
  switch (e.message) {
  | s when s->Js.String2.includes(wrongSecretKey) => WrongPassword
  | s when s->Js.String2.includes(branchRefused) => BranchRefused
  | s when s->Js.String2.includes(badPkh) => BadPkh
  | s when s->Js.String2.includes(unregisteredDelegate) =>
    UnregisteredDelegate
  | s when s->Js.String2.includes(unchangedDelegate) => UnchangedDelegate
  | s when s->Js.String2.includes(invalidContract) => InvalidContract
  | s when s->Js.String2.includes(emptyTransaction) => EmptyTransaction
  | s when s->Js.String2.includes(ledgerTimeout) => LedgerInitTimeout
  | s when s->Js.String2.includes(ledgerKey) => LedgerKeyRetrieval
  | s when s->Js.String2.includes(ledgerDenied) => LedgerDenied
  | s => Errors.Generic(Js.String.make(s))
  };

let () =
  Errors.registerHandler(
    "Taquito",
    fun
    | WrongPassword => I18n.form_input_error#wrong_password->Some
    | UnregisteredDelegate => I18n.form_input_error#unregistered_delegate->Some
    | UnchangedDelegate => I18n.form_input_error#change_baker->Some
    | BadPkh => I18n.form_input_error#bad_pkh->Some
    | BranchRefused => I18n.form_input_error#branch_refused_error->Some
    | InvalidContract => I18n.form_input_error#invalid_contract->Some
    | EmptyTransaction => I18n.form_input_error#empty_transaction->Some
    | LedgerInit(_) => I18n.form_input_error#hardware_wallet_plug->Some
    | LedgerNotReady => I18n.form_input_error#hardware_wallet_not_ready->Some
    | LedgerInitTimeout => I18n.form_input_error#hardware_wallet_plug->Some
    | LedgerKeyRetrieval =>
      I18n.form_input_error#hardware_wallet_check_app->Some
    | LedgerDenied => I18n.title#hardware_wallet_denied->Some
    | SignerIntentInconsistency =>
      I18n.form_input_error#hardware_wallet_signer_inconsistent->Some
    | _ => None,
  );

let fromPromiseParsed = res => RawJsError.fromPromiseParsed(res, parse);
