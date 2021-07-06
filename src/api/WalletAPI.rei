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
  type t = {
    name: string,
    derivationPath: DerivationPath.Pattern.t,
    addresses: array(string),
    legacyAddress: option(string),
  };

  let decoder: Js.Json.t => t;

  let encoder: t => Js.Json.t;
};

/** Aliases management */
module Aliases: {
  type t = array((string, string));

  let get: (~settings: AppSettings.t) => Future.t(Result.t(t, string));

  let getAliasForAddress:
    (~settings: AppSettings.t, ~address: string) =>
    Future.t(Result.t(option(string), string));

  let getAddressForAlias:
    (~settings: AppSettings.t, ~alias: string) =>
    Future.t(Result.t(option(string), string));

  let add:
    (~settings: AppSettings.t, ~alias: string, ~address: string) =>
    Future.t(Result.t(unit, string));

  let delete:
    (~settings: AppSettings.t, ~alias: string) =>
    Future.t(Result.t(unit, string));

  let rename:
    (~settings: AppSettings.t, TezosSDK.renameParams) =>
    Future.t(Result.t(unit, string));
};

/** Accounts management */
module Accounts: {
  /** Representation of accounts list */
  type t = array(Secret.t);

  let secrets: (~settings: AppSettings.t) => option(t);

  let recoveryPhrases:
    (~settings: AppSettings.t) =>
    option(array(SecureStorage.Cipher.encryptedData));

  let get:
    (~settings: AppSettings.t) =>
    Future.t(Result.t(array((string, string)), string));

  let updateSecretAt:
    (~settings: AppSettings.t, Secret.t, int) =>
    Future.t(Result.t(unit, string));

  let recoveryPhraseAt:
    (~settings: AppSettings.t, int, ~password: string) =>
    Future.t(Result.t(string, string));

  let add:
    (~settings: AppSettings.t, ~alias: string, ~address: string) =>
    Future.t(result(unit, TezosSDK.Error.t));

  let import:
    (
      ~settings: AppSettings.t,
      ~alias: string,
      ~secretKey: string,
      ~password: string
    ) =>
    Future.t(Result.t(string, string));

  let derive:
    (
      ~settings: AppSettings.t,
      ~index: int,
      ~alias: string,
      ~password: string
    ) =>
    Future.t(Result.t(string, string));

  /* Delete the given account */
  let delete:
    (~settings: AppSettings.t, string) =>
    Future.t(Result.t(option(unit), string));

  let deleteSecretAt:
    (~settings: AppSettings.t, int) =>
    Future.t(Result.t(array(unit), string));

  let used: (AppSettings.t, string) => Future.t(Result.t(bool, string));

  let restore:
    (
      ~settings: AppSettings.t,
      ~backupPhrase: string,
      ~name: string,
      ~derivationPath: TezosClient.DerivationPath.Pattern.t=?,
      ~password: string,
      unit
    ) =>
    Future.t(Result.t((array(string), option(string)), string));

  let scanAll:
    (~settings: AppSettings.t, ~password: string) =>
    Future.t(Result.t(unit, string));

  let getPublicKey:
    (~settings: AppSettings.t, ~account: Account.t) =>
    Future.t(Result.t(string, Wallet.error));
};
