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

  let get: (~config: ConfigContext.env) => Future.t(Result.t(t, Errors.t));

  let getAliasForAddress:
    (~config: ConfigContext.env, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(option(string), Errors.t));

  let getAddressForAlias:
    (~config: ConfigContext.env, ~alias: string) =>
    Future.t(Result.t(option(PublicKeyHash.t), Errors.t));

  let add:
    (~config: ConfigContext.env, ~alias: string, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(unit, Errors.t));

  let delete:
    (~config: ConfigContext.env, ~alias: string) =>
    Future.t(Result.t(unit, Errors.t));

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename:
    (~config: ConfigContext.env, renameParams) =>
    Future.t(Result.t(unit, Errors.t));
};

/** Accounts management */
module Accounts: {
  /** Representation of accounts list */
  type t = array(Secret.Repr.t);

  type name = string;

  let secrets:
    (~config: ConfigContext.env) => result(t, TezosClient.Errors.t);

  let recoveryPhrases:
    unit => option(array(SecureStorage.Cipher.encryptedData));

  let get:
    (~config: ConfigContext.env) =>
    Future.t(
      Result.t(array((name, PublicKeyHash.t, Wallet.kind)), Errors.t),
    );

  let updateSecretAt:
    (~config: ConfigContext.env, Secret.Repr.t, int) =>
    Result.t(unit, Errors.t);

  let recoveryPhraseAt:
    (int, ~password: string) => Future.t(Result.t(string, Errors.t));

  let import:
    (
      ~config: ConfigContext.env,
      ~alias: name,
      ~secretKey: string,
      ~password: string
    ) =>
    Future.t(Result.t(PublicKeyHash.t, Errors.t));

  let derive:
    (
      ~config: ConfigContext.env,
      ~index: int,
      ~alias: name,
      ~password: string
    ) =>
    Future.t(Result.t(PublicKeyHash.t, Errors.t));

  /* Delete the given account */
  let delete:
    (~config: ConfigContext.env, string) =>
    Future.t(Result.t(unit, Errors.t));

  let deleteSecretAt:
    (~config: ConfigContext.env, int) => Future.t(Result.t(unit, Errors.t));

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

    let used:
      (ConfigContext.env, PublicKeyHash.t) =>
      Future.t(Result.t(bool, Errors.t));

    let runStreamLedger:
      (
        ~config: ConfigContext.env,
        ~startIndex: int=?,
        ~onFoundKey: (int, PublicKeyHash.t) => unit,
        DerivationPath.Pattern.t,
        Wallet.Ledger.scheme
      ) =>
      Future.t(Belt.Result.t(unit, Errors.t));

    let runStreamSeed:
      (
        ~config: ConfigContext.env,
        ~startIndex: int=?,
        ~onFoundKey: (int, account(string)) => unit,
        ~password: string,
        Secret.Repr.derived,
        DerivationPath.Pattern.t
      ) =>
      Future.t(Belt.Result.t(unit, Errors.t));
  };

  let restore:
    (
      ~config: ConfigContext.env,
      ~backupPhrase: array(string),
      ~name: name,
      ~derivationPath: TezosClient.DerivationPath.Pattern.t=?,
      ~derivationScheme: Wallet.Ledger.scheme=?,
      ~password: string,
      unit
    ) =>
    Future.t(Result.t(unit, Errors.t));

  let importMnemonicKeys:
    (
      ~config: ConfigContext.env,
      ~accounts: list(Scan.account(string)),
      ~password: string,
      ~index: int,
      unit
    ) =>
    Future.t(
      Result.t(
        (array(PublicKeyHash.t), option(PublicKeyHash.t)),
        Errors.t,
      ),
    );

  let legacyImport:
    (~config: ConfigContext.env, string, string, ~password: string) =>
    Future.t(Belt.Result.t(PublicKeyHash.t, Errors.t));

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
    Future.t(Result.t(array(PublicKeyHash.t), Errors.t));

  let deriveLedgerKeys:
    (
      ~config: ConfigContext.env,
      ~timeout: int=?,
      ~index: int,
      ~accountsNumber: int,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Future.t(Result.t(array(PublicKeyHash.t), Errors.t));

  let deriveLedger:
    (
      ~config: ConfigContext.env,
      ~timeout: int=?,
      ~index: int,
      ~alias: string,
      ~ledgerMasterKey: PublicKeyHash.t,
      unit
    ) =>
    Future.t(Result.t(PublicKeyHash.t, Errors.t));

  let getPublicKey:
    (~config: ConfigContext.env, ~account: Account.t) =>
    Future.t(Result.t(string, Errors.t));
};
