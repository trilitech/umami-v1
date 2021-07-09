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

/** Wallet API layer with Wallet backend */

/** Secret representation in LocalStorage */
module Secret: {
  module Repr = Secret;
  type t = Repr.t;

  let decoder: Js.Json.t => t;

  let encoder: t => Js.Json.t;
};

/** Aliases management */
module Aliases: {
  type t = array((string, PublicKeyHash.t));

  let get: (~config: ConfigFile.t) => Future.t(Result.t(t, ErrorHandler.t));

  let getAliasForAddress:
    (~config: ConfigFile.t, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(option(string), ErrorHandler.t));

  let getAddressForAlias:
    (~config: ConfigFile.t, ~alias: string) =>
    Future.t(Result.t(option(PublicKeyHash.t), ErrorHandler.t));

  let add:
    (~config: ConfigFile.t, ~alias: string, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let delete:
    (~config: ConfigFile.t, ~alias: string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename:
    (~config: ConfigFile.t, renameParams) =>
    Future.t(Result.t(unit, ErrorHandler.t));
};

/** Accounts management */
module Accounts: {
  /** Representation of accounts list */
  type t = array(Secret.Repr.t);

  type name = string;

  let secrets:
    (~config: ConfigFile.t) => result(t, TezosClient.ErrorHandler.t);

  let recoveryPhrases:
    (~config: ConfigFile.t) =>
    option(array(SecureStorage.Cipher.encryptedData));

  let get:
    (~config: ConfigFile.t) =>
    Future.t(Result.t(array((name, PublicKeyHash.t)), ErrorHandler.t));

  let updateSecretAt:
    (~config: ConfigFile.t, Secret.Repr.t, int) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let recoveryPhraseAt:
    (~config: ConfigFile.t, int, ~password: string) =>
    Future.t(Result.t(string, ErrorHandler.t));

  let import:
    (
      ~config: ConfigFile.t,
      ~alias: name,
      ~secretKey: string,
      ~password: string
    ) =>
    Future.t(Result.t(PublicKeyHash.t, ErrorHandler.t));

  let derive:
    (~config: ConfigFile.t, ~index: int, ~alias: name, ~password: string) =>
    Future.t(Result.t(PublicKeyHash.t, ErrorHandler.t));

  /* Delete the given account */
  let delete:
    (~config: ConfigFile.t, string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let deleteSecretAt:
    (~config: ConfigFile.t, int) =>
    Future.t(Result.t(array(unit), ErrorHandler.t));

  let used:
    (ConfigFile.t, PublicKeyHash.t) =>
    Future.t(Result.t(bool, ErrorHandler.t));

  let restore:
    (
      ~config: ConfigFile.t,
      ~backupPhrase: array(string),
      ~name: name,
      ~derivationPath: TezosClient.DerivationPath.Pattern.t=?,
      ~derivationScheme: Wallet.Ledger.scheme=?,
      ~password: string,
      unit
    ) =>
    Future.t(
      Result.t(
        (array(PublicKeyHash.t), option(PublicKeyHash.t)),
        ErrorHandler.t,
      ),
    );

  let legacyImport:
    (~config: ConfigUtils.t, string, string, ~password: string) =>
    Future.t(Belt.Result.t(PublicKeyHash.t, ErrorHandler.t));

  let importLedger:
    (
      ~config: ConfigUtils.t,
      ~name: string,
      ~accountsNumber: int,
      ~derivationPath: DerivationPath.Pattern.t=?,
      ~derivationScheme: Wallet.Ledger.scheme=?,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Future.t(Result.t(array(PublicKeyHash.t), ErrorHandler.t));

  let scanAll:
    (~config: ConfigFile.t, ~password: string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let getPublicKey:
    (~config: ConfigFile.t, ~account: Account.t) =>
    Future.t(Result.t(string, Wallet.error));
};
