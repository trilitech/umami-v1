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
let balanceTooLow = "contract.balance_too_low";
let unchangedDelegate = "contract.manager.delegate.unchanged";
let invalidContract = "Invalid contract notation";
let emptyTransaction = "contract.empty_transaction";
let ledgerTimeout = "No Ledger device found (timeout)";
let ledgerKey = "Unable to retrieve public key";
let ledgerDenied = "0x6985";
let emptyContract = "implicit.empty_implicit_contract";
let scriptRejected = "script_rejected";
let noMetadata = "Non-compliance with the TZIP-016 standard.";
let noTokenMetadata = "No token metadata";
let tokenIdNotFound = "Could not find token metadata for the token ID";
let requestFailed1 = "Request to";
let requestFailed2 = "failed";
let gasExhausted = "gas_exhausted.operation";
let storageExhausted = "storage_exhausted.operation";
let gasLimitTooHigh = "gas_limit_too_high";
let storageLimitTooHigh = "storage_limit_too_high";

type Errors.t +=
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
  | BalanceTooLow
  | EmptyContract
  | ScriptRejected
  | SignerIntentInconsistency
  | NoMetadata
  | NoTokenMetadata
  | NodeRequestFailed
  | TokenIdNotFound
  | ParseScript(option(string))
  | ParseMicheline(option(string))
  | GasExhausted
  | StorageExhausted
  | GasExhaustedAboveLimit
  | StorageExhaustedAboveLimit;

let parse = (e: RawJsError.t) =>
  switch (e.message) {
  | s when s->Js.String2.includes(wrongSecretKey) => SecureStorage.WrongPassword
  | s when s->Js.String2.includes(branchRefused) => BranchRefused
  | s when s->Js.String2.includes(badPkh) => BadPkh
  | s when s->Js.String2.includes(unregisteredDelegate) =>
    UnregisteredDelegate
  | s when s->Js.String2.includes(unchangedDelegate) => UnchangedDelegate
  | s when s->Js.String2.includes(balanceTooLow) => BalanceTooLow
  | s when s->Js.String2.includes(emptyContract) => EmptyContract
  | s when s->Js.String2.includes(scriptRejected) => ScriptRejected
  | s when s->Js.String2.includes(invalidContract) => InvalidContract
  | s when s->Js.String2.includes(emptyTransaction) => EmptyTransaction
  | s when s->Js.String2.includes(ledgerTimeout) => LedgerInitTimeout
  | s when s->Js.String2.includes(ledgerKey) => LedgerKeyRetrieval
  | s when s->Js.String2.includes(ledgerDenied) => LedgerDenied
  | s when s->Js.String2.includes(noMetadata) => NoMetadata
  | s when s->Js.String2.includes(noTokenMetadata) => NoTokenMetadata
  | s when s->Js.String2.includes(tokenIdNotFound) => TokenIdNotFound
  | s when s->Js.String2.includes(gasExhausted) => GasExhausted
  | s when s->Js.String2.includes(storageExhausted) => StorageExhausted
  | s when s->Js.String2.includes(gasLimitTooHigh) => GasExhaustedAboveLimit
  | s when s->Js.String2.includes(storageLimitTooHigh) =>
    StorageExhaustedAboveLimit
  | s
      when
        s->Js.String2.startsWith(requestFailed1)
        && s->Js.String2.endsWith(requestFailed2) =>
    NodeRequestFailed

  | s => Errors.Generic(Js.String.make(s))
  };

let () =
  Errors.registerHandler(
    "Taquito",
    fun
    | UnregisteredDelegate => I18n.Form_input_error.unregistered_delegate->Some
    | UnchangedDelegate => I18n.Form_input_error.change_baker->Some
    | BadPkh => I18n.Form_input_error.bad_pkh->Some
    | BranchRefused => I18n.Form_input_error.branch_refused_error->Some
    | InvalidContract => I18n.Form_input_error.invalid_contract->Some
    | BalanceTooLow => I18n.Form_input_error.balance_too_low->Some
    | ScriptRejected => I18n.Form_input_error.balance_too_low->Some
    | EmptyContract => I18n.Form_input_error.account_balance_empty->Some
    | EmptyTransaction => I18n.Form_input_error.empty_transaction->Some
    | LedgerInit(_) => I18n.Form_input_error.hardware_wallet_plug->Some
    | LedgerNotReady => I18n.Form_input_error.hardware_wallet_not_ready->Some
    | LedgerInitTimeout => I18n.Form_input_error.hardware_wallet_plug->Some
    | LedgerKeyRetrieval =>
      I18n.Form_input_error.hardware_wallet_check_app->Some
    | LedgerDenied => I18n.Title.hardware_wallet_denied->Some
    | SignerIntentInconsistency =>
      I18n.Form_input_error.hardware_wallet_signer_inconsistent->Some
    | NoMetadata => I18n.Form_input_error.no_metadata(None)->Some
    | NoTokenMetadata => I18n.Form_input_error.no_token_metadata(None)->Some
    | TokenIdNotFound => I18n.Form_input_error.token_id_not_found(None)->Some
    | NodeRequestFailed => I18n.Errors.request_to_node_failed->Some
    | ParseScript(e) => I18n.Errors.script_parsing(e)->Some
    | ParseMicheline(e) => I18n.Errors.micheline_parsing(e)->Some
    | GasExhausted => I18n.Errors.gas_exhausted->Some
    | StorageExhausted => I18n.Errors.storage_exhausted->Some
    | GasExhaustedAboveLimit => I18n.Errors.gas_exhausted_above_limit->Some
    | StorageExhaustedAboveLimit =>
      I18n.Errors.storage_exhausted_above_limit->Some
    | _ => None,
  );

let fromPromiseParsed = res => RawJsError.fromPromiseParsed(res, parse);
