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

  let get: (~config: ConfigContext.env) => Promise.t(t);

  let getAliasForAddress:
    (~config: ConfigContext.env, ~address: PublicKeyHash.t) =>
    Promise.t(option(string));

  let getAddressForAlias:
    (~config: ConfigContext.env, ~alias: string) =>
    Promise.t(option(PublicKeyHash.t));

  let add:
    (~config: ConfigContext.env, ~alias: string, ~address: PublicKeyHash.t) =>
    Promise.t(unit);

  let delete: (~config: ConfigContext.env, ~alias: string) => Promise.t(unit);

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename: (~config: ConfigContext.env, renameParams) => Promise.t(unit);
};

/** Accounts management */
module Accounts: {
  /** Representation of accounts list */
  type t = array(Secret.Repr.t);

  type name = string;

  let secrets: (~config: ConfigContext.env) => Promise.result(t);

  let recoveryPhrases:
    unit => Promise.result(array(SecureStorage.Cipher.encryptedData));

  let get:
    (~config: ConfigContext.env) =>
    Promise.t(array((name, PublicKeyHash.t, Wallet.kind)));

  let updateSecretAt:
    (~config: ConfigContext.env, Secret.Repr.t, int) => Promise.result(unit);

  let recoveryPhraseAt: (int, ~password: string) => Promise.t(string);

  let import:
    (
      ~config: ConfigContext.env,
      ~alias: name,
      ~secretKey: string,
      ~password: string
    ) =>
    Promise.t(PublicKeyHash.t);

  let derive:
    (
      ~config: ConfigContext.env,
      ~index: int,
      ~alias: name,
      ~password: string
    ) =>
    Promise.t(PublicKeyHash.t);

  /* Delete the given account */
  let delete: (~config: ConfigContext.env, string) => Promise.t(unit);

  let deleteSecretAt: (~config: ConfigContext.env, int) => Promise.t(unit);

  module Scan: {
    type Errors.t +=
      | APIError(string);

    type kind =
      | Regular
      | Legacy;

    type account('a) = {
      kind,
      publicKeyHash: PublicKeyHash.t,
      encryptedSecretKey: 'a,
    };

    let used: (ConfigContext.env, PublicKeyHash.t) => Promise.t(bool);

    let runStreamLedger:
      (
        ~config: ConfigContext.env,
        ~startIndex: int=?,
        ~onFoundKey: (int, PublicKeyHash.t) => unit,
        DerivationPath.Pattern.t,
        Wallet.Ledger.scheme
      ) =>
      Promise.t(unit);

    let runStreamSeed:
      (
        ~config: ConfigContext.env,
        ~startIndex: int=?,
        ~onFoundKey: (int, account(string)) => unit,
        ~password: string,
        Secret.Repr.derived,
        DerivationPath.Pattern.t
      ) =>
      Promise.t(unit);
  };

  let restore:
    (
      ~config: ConfigContext.env,
      ~backupPhrase: array(string),
      ~name: name,
      ~derivationPath: DerivationPath.Pattern.t=?,
      ~derivationScheme: Wallet.Ledger.scheme=?,
      ~password: string,
      unit
    ) =>
    Promise.t(unit);

  let restoreFromBackupFile:
    (
      ~config: ConfigContext.env,
      ~backupFile: System.Path.t,
      ~password: string,
      unit
    ) =>
    Promise.t(unit);

  let forceBackup: (~config: ConfigContext.env) => Promise.result(unit);

  let importMnemonicKeys:
    (
      ~config: ConfigContext.env,
      ~accounts: list(Scan.account(string)),
      ~password: string,
      ~index: int,
      unit
    ) =>
    Promise.t((array(PublicKeyHash.t), option(PublicKeyHash.t)));

  let legacyImport:
    (~config: ConfigContext.env, string, string, ~password: string) =>
    Promise.t(PublicKeyHash.t);

  let importLedger:
    (
      ~config: ConfigContext.env,
      ~timeout: int=?,
      ~name: string,
      ~accountsNumber: int,
      ~derivationPath: DerivationPath.Pattern.t=?,
      ~derivationScheme: Wallet.Ledger.scheme=?,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Promise.t(array(PublicKeyHash.t));

  let deriveLedgerKeys:
    (
      ~config: ConfigContext.env,
      ~timeout: int=?,
      ~index: int,
      ~accountsNumber: int,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Promise.t(array(PublicKeyHash.t));

  let deriveLedger:
    (
      ~config: ConfigContext.env,
      ~timeout: int=?,
      ~index: int,
      ~alias: string,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Promise.t(PublicKeyHash.t);

  let getPublicKey:
    (~config: ConfigContext.env, ~account: Account.t) => Promise.t(string);
};
