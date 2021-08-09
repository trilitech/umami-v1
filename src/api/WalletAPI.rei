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

/** Wallet API layer with TezosSDK/Wallet backend */

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

  let get:
    (~settings: AppSettings.t) => Future.t(Result.t(t, ErrorHandler.t));

  let getAliasForAddress:
    (~settings: AppSettings.t, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(option(string), ErrorHandler.t));

  let getAddressForAlias:
    (~settings: AppSettings.t, ~alias: string) =>
    Future.t(Result.t(option(PublicKeyHash.t), ErrorHandler.t));

  let add:
    (~settings: AppSettings.t, ~alias: string, ~address: PublicKeyHash.t) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let delete:
    (~settings: AppSettings.t, ~alias: string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let rename:
    (~settings: AppSettings.t, TezosSDK.renameParams) =>
    Future.t(Result.t(unit, ErrorHandler.t));
};

/** Accounts management */
module Accounts: {
  /** Representation of accounts list */
  type t = array(Secret.Repr.t);

  type name = string;

  let secrets:
    (~settings: AppSettings.t) => result(t, TezosClient.ErrorHandler.t);

  let recoveryPhrases:
    (~settings: AppSettings.t) =>
    option(array(SecureStorage.Cipher.encryptedData));

  let get:
    (~settings: AppSettings.t) =>
    Future.t(Result.t(array((name, PublicKeyHash.t)), ErrorHandler.t));

  let updateSecretAt:
    (~settings: AppSettings.t, Secret.Repr.t, int) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let recoveryPhraseAt:
    (~settings: AppSettings.t, int, ~password: string) =>
    Future.t(Result.t(string, ErrorHandler.t));

  let add:
    (~settings: AppSettings.t, ~alias: name, ~address: PublicKeyHash.t) =>
    Future.t(result(unit, TezosSDK.Error.t));

  let import:
    (
      ~settings: AppSettings.t,
      ~alias: name,
      ~secretKey: string,
      ~password: string
    ) =>
    Future.t(Result.t(PublicKeyHash.t, ErrorHandler.t));

  let derive:
    (~settings: AppSettings.t, ~index: int, ~alias: name, ~password: string) =>
    Future.t(Result.t(PublicKeyHash.t, ErrorHandler.t));

  /* Delete the given account */
  let delete:
    (~settings: AppSettings.t, string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let deleteSecretAt:
    (~settings: AppSettings.t, int) =>
    Future.t(Result.t(array(unit), ErrorHandler.t));

  let used:
    (AppSettings.t, PublicKeyHash.t) =>
    Future.t(Result.t(bool, ErrorHandler.t));

  let restore:
    (
      ~settings: AppSettings.t,
      ~backupPhrase: string,
      ~name: name,
      ~derivationPath: TezosClient.DerivationPath.Pattern.t=?,
      ~password: string,
      unit
    ) =>
    Future.t(
      Result.t(
        (array(PublicKeyHash.t), option(PublicKeyHash.t)),
        ErrorHandler.t,
      ),
    );

  let scanAll:
    (~settings: AppSettings.t, ~password: string) =>
    Future.t(Result.t(unit, ErrorHandler.t));

  let getPublicKey:
    (~settings: AppSettings.t, ~account: Account.t) =>
    Future.t(Result.t(string, Wallet.error));
};
