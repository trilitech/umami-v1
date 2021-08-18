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

type token =
  | OperationNotRunnableOffchain(string)
  | SimulationNotAvailable(string)
  | InjectionNotImplemented(string)
  | OffchainCallNotImplemented(string)
  | RawError(string);

type wallet = Wallet.error;

type walletAPI =
  | NoSecretFound
  | SecretNotFound(int)
  | CannotUpdateSecret(int)
  | RecoveryPhraseNotFound(int)
  | SecretAlreadyImported
  | IncorrectNumberOfWords
  | UnknownBip39Word(string, int)
  | Generic(string);

type t =
  | Taquito(ReTaquitoError.t)
  | Token(token)
  | Wallet(wallet)
  | WalletAPI(walletAPI);

let taquito = e => Taquito(e);
let token = e => Token(e);

let fromTaquitoToString = e =>
  e->ReTaquitoError.(
       fun
       | Generic(s) => s
       | WrongPassword => I18n.form_input_error#wrong_password
       | UnregisteredDelegate => I18n.form_input_error#unregistered_delegate
       | UnchangedDelegate => I18n.form_input_error#change_baker
       | BadPkh => I18n.form_input_error#bad_pkh
       | BranchRefused => I18n.form_input_error#branch_refused_error
       | InvalidContract => I18n.form_input_error#invalid_contract
       | EmptyTransaction => I18n.form_input_error#empty_transaction
       | LedgerInit(_) => I18n.form_input_error#hardware_wallet_plug
       | LedgerNotReady => I18n.form_input_error#hardware_wallet_not_ready
       | LedgerMasterKeyRetrieval(_) =>
         I18n.form_input_error#hardware_wallet_check_app
     );

let printError = (fmt, err) => {
  switch (err) {
  | OperationNotRunnableOffchain(s) =>
    Format.fprintf(fmt, "Operation '%s' cannot be run offchain.", s)
  | SimulationNotAvailable(s) =>
    Format.fprintf(fmt, "Operation '%s' is not simulable.", s)
  | InjectionNotImplemented(s) =>
    Format.fprintf(fmt, "Operation '%s' injection is not implemented", s)
  | OffchainCallNotImplemented(s) =>
    Format.fprintf(fmt, "Operation '%s' offchain call is not implemented", s)
  | RawError(s) => Format.fprintf(fmt, "%s", s)
  };
};

let fromTokenToString = err => Format.asprintf("%a", printError, err);

let fromWalletToString =
  fun
  | Wallet.KeyNotFound => I18n.wallet#key_not_found
  | File(e) => e->System.File.Error.toString
  | LedgerParsingError(s)
  | Generic(s) => s;

let fromWalletAPIToString =
  fun
  | NoSecretFound => I18n.errors#no_secret_found
  | SecretNotFound(i) => I18n.errors#secret_not_found(i)
  | CannotUpdateSecret(i) => I18n.errors#cannot_update_secret(i)
  | RecoveryPhraseNotFound(i) => I18n.errors#recovery_phrase_not_found(i)
  | SecretAlreadyImported => I18n.errors#secret_already_imported
  | IncorrectNumberOfWords => I18n.errors#incorrect_number_of_words
  | UnknownBip39Word(w, i) => I18n.errors#unknown_bip39_word(w, i)
  | Generic(s) => s;

let toString =
  fun
  | Token(e) => fromTokenToString(e)
  | Taquito(e) => fromTaquitoToString(e)
  | Wallet(e) => fromWalletToString(e)
  | WalletAPI(e) => fromWalletAPIToString(e);

type promiseError = {
  message: string,
  name: string,
};

external extractPromiseError: Js.Promise.error => promiseError = "%identity";
