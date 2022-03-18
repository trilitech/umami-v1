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

type Errors.t +=
  | InvalidEncoding(string)
  | KeyBadFormat(string)
  | KeyNotFound;

/** Value and file associated to a kind of alias. */
module type AliasType = {
  type t;
  let filename: System.Path.t;
};

/** Signature of an aliases file reader/writer. */
module type AliasesMakerType = {
  type key;
  type value;
  type alias = {
    name: key,
    value,
  };
  type t = array(alias);
  let parse: string => t;
  let stringify: t => string;
  let read: System.Path.t => Promise.t(t);
  let write: (System.Path.t, t) => Promise.t(unit);
  let protect: (System.Path.t, unit => Promise.t(unit)) => Promise.t(unit);
  let find: (t, alias => bool) => Result.t(alias, Errors.t);
  let filter: (t, alias => bool) => t;
  let remove: (t, key) => t;
  let addOrReplace: (t, key, value) => t;
  let rename: (t, ~oldName: key, ~newName: key) => t;
};

/** Secret key representation */
module SecretAlias: {type t = string;};

module SecretAliases:
  AliasesMakerType with type key := string and type value := SecretAlias.t;

/** Public key representation */
module PkAlias: {type t;};

module PkAliases:
  AliasesMakerType with type key := string and type value := PkAlias.t;

/** Public key hash representation */
module PkhAlias: {type t = PublicKeyHash.t;};

module PkhAliases:
  AliasesMakerType with type key := string and type value := PkhAlias.t;

/** Add or replace a public key hash alias. */
let addOrReplacePkhAlias:
  (~dirpath: System.Path.t, ~alias: string, ~pkh: PkhAlias.t) =>
  Promise.t(unit);

/** Remove an alias with its associated pkh. */
let removePkhAlias:
  (~dirpath: System.Path.t, ~alias: string) => Promise.t(unit);

/** Rename an alias with its associated public, private and pkh. */
let renamePkhAlias:
  (~dirpath: System.Path.t, ~oldName: string, ~newName: string) =>
  Promise.t(unit);

/** Add or replace an alias with its associated public, private and pkh. */
let addOrReplaceAlias:
  (
    ~dirpath: System.Path.t,
    ~alias: string,
    ~pk: PkAlias.t,
    ~pkh: PkhAlias.t,
    ~sk: SecretAlias.t
  ) =>
  Promise.t(unit);

/** Remove an alias from the filesystem. */
let removeAlias: (~dirpath: System.Path.t, ~alias: string) => Promise.t(unit);

/** Rename an alias with its associated public, private and pkh. */
let renameAlias:
  (~dirpath: System.Path.t, ~oldName: string, ~newName: string) =>
  Promise.t(unit);

type kind =
  Account.kind =
    | Encrypted | Unencrypted | Ledger | CustomAuth(ReCustomAuth.infos);

module Prefixes: {
  type t =
    | Encrypted
    | Unencrypted
    | Ledger
    | CustomAuth;

  let toString: t => string;
};

/** Returns the prefix kind from the secret key and the secret key without the
   prefix */
let extractPrefixFromSecretKey: string => Promise.result((kind, string));

/** Returns the secret key associated to a public key hash. */
let readSecretFromPkh:
  (PkhAlias.t, System.Path.t) => Promise.t((kind, SecretAlias.t));

/** Returns the alias associated to a public key hash */
let aliasFromPkh:
  (~dirpath: System.Path.t, ~pkh: PkhAlias.t) => Promise.t(string);

/** Returns the public key associated to an alias */
let pkFromAlias:
  (~dirpath: System.Path.t, ~alias: string) => Promise.t(string);

let mnemonicPkValue: string => PkAlias.t;

let customPkValue: (~secretPath: string, string) => PkAlias.t;

module Ledger: {
  type Errors.t +=
    | InvalidPathSize(array(int))
    | InvalidIndex(int, string)
    | InvalidLedger(string);

  type t = {
    path: DerivationPath.tezosBip44,
    scheme: PublicKeyHash.Scheme.t,
  };

  /** The "master key" of a ledger is a way to give it an identity:
     it's the public key at path `44'/1729'` and scheme ED25519. This
     specific public key hash is used by `tezos-client` to give it its
     "animal" prefix. The masterkey is required to encode the "private
     key". */
  type masterKey = PublicKeyHash.t;

  let masterKeyPath: DerivationPath.t;
  let masterKeyScheme: PublicKeyHash.Scheme.t;

  module Decode: {
    let fromSecretKey:
      (SecretAlias.t, ~ledgerBasePkh: PublicKeyHash.t) => result(t, Errors.t);
  };

  module Encode: {
    let toSecretKey: (t, ~ledgerBasePkh: PublicKeyHash.t) => SecretAlias.t;
  };
};

module CustomAuth: {
  module Decode: {
    let fromSecretKey: SecretAlias.t => Let.result(ReCustomAuth.infos);
  };
  module Encode: {let toSecretKey: ReCustomAuth.infos => SecretAlias.t;};
};
