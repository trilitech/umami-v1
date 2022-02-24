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

open System.Path.Ops;
open Let;

type Errors.t +=
  | KeyNotFound
  | KeyBadFormat(string)
  | InvalidEncoding(string);

let () =
  Errors.registerHandler(
    "Wallet",
    fun
    | KeyNotFound => I18n.Wallet.key_not_found->Some
    | KeyBadFormat(s) => I18n.Wallet.key_bad_format(s)->Some
    | InvalidEncoding(e) => I18n.Wallet.invalid_encoding(e)->Some
    | _ => None,
  );

module type AliasType = {
  type t;
  let filename: System.Path.t;
};

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
  let find: (t, alias => bool) => Promise.result(alias);
  let filter: (t, alias => bool) => t;
  let remove: (t, key) => t;
  let addOrReplace: (t, key, value) => t;
  let rename: (t, ~oldName: key, ~newName: key) => t;
};

module AliasesMaker =
       (Alias: AliasType)
       : (AliasesMakerType with type key := string and type value := Alias.t) => {
  type alias = {
    name: string,
    value: Alias.t,
  };

  type t = array(alias);

  [@bs.val] [@bs.scope "JSON"] external parse: string => t = "parse";
  [@bs.val] [@bs.scope "JSON"] external stringify: t => string = "stringify";

  let read = dirpath =>
    System.File.read(dirpath / Alias.filename)->Promise.mapOk(parse);

  let write = (dirpath, aliases) =>
    System.File.write(~name=dirpath / Alias.filename, stringify(aliases));

  let protect = (dirpath, f) =>
    System.File.protect(~name=dirpath / Alias.filename, ~transaction=f);

  let find = (aliases: t, f) =>
    aliases->Js.Array2.find(f)->Result.fromOption(KeyNotFound);

  let filter = (aliases, f) => aliases->Js.Array2.filter(f);

  let remove = (aliases, aliasName) => {
    aliases->filter(a => a.name != aliasName);
  };

  let addOrReplace = (aliases, alias, value) => {
    let alias = {name: alias, value};
    aliases->remove(alias.name)->Js.Array.concat([|alias|]);
  };

  let rename = (aliases, ~oldName, ~newName) => {
    aliases->Js.Array2.map(alias =>
      alias.name == oldName ? {...alias, name: newName} : alias
    );
  };
};

module SecretAlias = {
  type t = string;
};

module SecretAliases =
  AliasesMaker({
    type t = SecretAlias.t;
    let filename = !"secret_keys";
  });

module PkAlias = {
  type t = {
    locator: string,
    key: string,
  };
};

module PkAliases =
  AliasesMaker({
    type t = PkAlias.t;

    let filename = !"public_keys";
  });

module PkhAlias = {
  type t = PublicKeyHash.t;
};

module PkhAliases =
  AliasesMaker({
    type t = PkhAlias.t;

    let filename = !"public_key_hashs";
  });

let aliasFromPkh = (~dirpath, ~pkh) => {
  dirpath
  ->PkhAliases.read
  ->Promise.flatMapOk(pkhaliases =>
      pkhaliases
      ->PkhAliases.find(a => a.value == pkh)
      ->Result.map(a => a.name)
      ->Promise.value
    );
};

let pkFromAlias = (~dirpath, ~alias) => {
  let%AwaitRes pkaliases = dirpath->PkAliases.read;
  let%ResMap a = pkaliases->PkAliases.find(a => a.name == alias);
  a.value.PkAlias.key;
};

let updatePkhAlias = (~dirpath, ~update) =>
  dirpath->PkhAliases.protect(_ => {
    let%Await pkhAliases = PkhAliases.read(dirpath);
    let pkhAliases = update(pkhAliases);
    dirpath->PkhAliases.write(pkhAliases);
  });

let addOrReplacePkhAlias = (~dirpath, ~alias, ~pkh) => {
  updatePkhAlias(~dirpath, ~update=pkhs =>
    PkhAliases.addOrReplace(pkhs, alias, pkh)
  );
};

let removePkhAlias = (~dirpath, ~alias) => {
  updatePkhAlias(~dirpath, ~update=pkhs => PkhAliases.remove(pkhs, alias));
};

let renamePkhAlias = (~dirpath, ~oldName, ~newName) => {
  updatePkhAlias(~dirpath, ~update=pkhs =>
    PkhAliases.rename(pkhs, ~oldName, ~newName)
  );
};

let protectAliases = (~dirpath, ~f) => {
  dirpath->PkAliases.protect(_ =>
    dirpath->PkhAliases.protect(_ => dirpath->SecretAliases.protect(f))
  );
};

let updateAlias = (~dirpath, ~update) => {
  let update = () => {
    let%Await pkAliases = PkAliases.read(dirpath);
    let%Await pkhAliases = PkhAliases.read(dirpath);
    let%Await skAliases = SecretAliases.read(dirpath);
    let (pks, pkhs, sks) = update(pkAliases, pkhAliases, skAliases);
    let%Await () = PkAliases.write(dirpath, pks);
    let%Await () = PkhAliases.write(dirpath, pkhs);
    SecretAliases.write(dirpath, sks);
  };
  protectAliases(~dirpath, ~f=update);
};

let addOrReplaceAlias = (~dirpath, ~alias, ~pk, ~pkh, ~sk) => {
  let update = (pks, pkhs, sks) => (
    PkAliases.addOrReplace(pks, alias, pk),
    PkhAliases.addOrReplace(pkhs, alias, pkh),
    SecretAliases.addOrReplace(sks, alias, sk),
  );

  updateAlias(~dirpath, ~update);
};

let removeAlias = (~dirpath, ~alias) => {
  let update = (pks, pkhs, sks) => (
    PkAliases.remove(pks, alias),
    PkhAliases.remove(pkhs, alias),
    SecretAliases.remove(sks, alias),
  );

  updateAlias(~dirpath, ~update);
};

let renameAlias = (~dirpath, ~oldName, ~newName) => {
  let update = (pks, pkhs, sks) => (
    PkAliases.rename(pks, ~oldName, ~newName),
    PkhAliases.rename(pkhs, ~oldName, ~newName),
    SecretAliases.rename(sks, ~oldName, ~newName),
  );

  updateAlias(~dirpath, ~update);
};

module CustomAuth = {
  module Decode = {
    let fromSecretKey = uri => {
      let elems = uri->Js.String2.split("/");

      let%ResMap (provider, handle) =
        switch (elems) {
        | [|provider, handle|] =>
          let%ResMap provider = provider->ReCustomAuthType.providerFromString;
          (provider, handle->ReCustomAuthType.Handle.fromString);

        | _ => Error(InvalidEncoding(uri))
        };

      ReCustomAuth.{provider, handle};
    };
  };

  module Encode = {
    open ReCustomAuth;
    let toSecretKey = t => {
      Format.sprintf(
        "customauth://%s/%s",
        t.provider->ReCustomAuth.providerToString,
        (t.handle :> string),
      );
    };
  };
};

type kind =
  Account.kind =
    | Encrypted | Unencrypted | Ledger | CustomAuth(ReCustomAuth.infos);

module Prefixes = {
  type t =
    | Encrypted
    | Unencrypted
    | Ledger
    | CustomAuth;

  let fromKind =
    fun
    | Account.Encrypted => Encrypted
    | Unencrypted => Unencrypted
    | Ledger => Ledger
    | CustomAuth(_) => CustomAuth;

  let toString =
    fun
    | Encrypted => "encrypted:"
    | Unencrypted => "unencrypted:"
    | Ledger => "ledger://"
    | CustomAuth => "customauth://";
};

let extractPrefixFromSecretKey = k => {
  let sub = (k, kind) =>
    k->Js.String2.substringToEnd(
      ~from=String.length(kind->Prefixes.toString),
    );

  let checkStart = (k, pref) =>
    k->Js.String2.startsWith(Prefixes.toString(pref));

  let buildRes = pref => Ok((pref, k->sub(pref->Prefixes.fromKind)));

  switch (k) {
  | k when checkStart(k, Prefixes.Encrypted) => buildRes(Encrypted)
  | k when checkStart(k, Prefixes.Unencrypted) => buildRes(Unencrypted)
  | k when checkStart(k, Prefixes.Ledger) => buildRes(Ledger)
  | k when checkStart(k, Prefixes.CustomAuth) =>
    let uri = sub(k, Prefixes.CustomAuth);
    let%ResMap infos = CustomAuth.Decode.fromSecretKey(uri);
    (CustomAuth(infos), uri);
  | k => Error(KeyBadFormat(k))
  };
};

let readSecretFromPkh = (address, dirpath) => {
  let%Await alias = aliasFromPkh(~dirpath, ~pkh=address);
  let%Await secretAliases = dirpath->SecretAliases.read;

  let%AwaitRes {value: k} =
    secretAliases
    ->Js.Array2.find(a => a.SecretAliases.name == alias)
    ->Promise.fromOption(~error=KeyNotFound);

  extractPrefixFromSecretKey(k);
};

let mnemonicPkValue = pk => PkAlias.{locator: "unencrypted:" ++ pk, key: pk};

let customPkValue = (~secretPath, pk) =>
  PkAlias.{locator: secretPath, key: pk};

module Ledger = {
  type Errors.t +=
    | InvalidPathSize(array(int))
    | InvalidIndex(int, string)
    | InvalidScheme(string)
    | InvalidLedger(string);

  let () =
    Errors.registerHandler(
      "Ledger",
      fun
      | InvalidPathSize(p) =>
        I18n.Wallet.invalid_path_size(p->Js.String.make)->Some
      | InvalidIndex(index, value) =>
        I18n.Wallet.invalid_index(index, value)->Some
      | InvalidScheme(s) => I18n.Wallet.invalid_scheme(s)->Some
      | InvalidLedger(p) => I18n.Wallet.invalid_ledger(p)->Some
      | _ => None,
    );

  /** Format of a ledger encoded secret key: `ledger://prefix/scheme/path`, where:
    - prefix is the public key derivated from the path `44'/1729'` and scheme
     `ed25519`, as a public key hash or encoded using the "crouching tiger"
      algorithm
    - scheme is either `ed25519`, `secp256k1` or `P-256`
    - path are the indexes after `44'/1279'` in a Tezos derivation path, where
      `'` has been substituted to `h`.
  */
  type secretKeyEncoding = string;

  type prefix =
    | Animals(string)
    | Pkh(string);

  type scheme =
    | ED25519
    | SECP256K1
    | P256;

  type t = {
    path: DerivationPath.tezosBip44,
    scheme,
  };

  type masterKey = PublicKeyHash.t;
  let masterKeyPath = DerivationPath.build([|44, 1729|]);
  let masterKeyScheme = ED25519;

  let schemeToString =
    fun
    | ED25519 => "ed25519"
    | SECP256K1 => "secp256k1"
    | P256 => "P-256";

  let schemeFromString =
    fun
    | "ed25519" => Ok(ED25519)
    | "secp256k1" => Ok(SECP256K1)
    | "P-256" => Ok(P256)
    | s => Error(InvalidScheme(s));

  type implicit =
    | TZ1
    | TZ2
    | TZ3;

  type kind =
    | Implicit(implicit)
    | KT1;

  let implicitFromScheme =
    fun
    | ED25519 => TZ1
    | SECP256K1 => TZ2
    | P256 => TZ3;

  let kindToString =
    fun
    | Implicit(TZ1) => "tz1"
    | Implicit(TZ2) => "tz2"
    | Implicit(TZ3) => "tz3"
    | KT1 => "kt1";

  module Decode = {
    let decodePrefix = (prefix, ledgerBasePkh: PublicKeyHash.t) =>
      switch (PublicKeyHash.build(prefix)) {
      | Ok(pkh) =>
        pkh == ledgerBasePkh
          ? Ok(Pkh(prefix))
          : Error(InvalidLedger((ledgerBasePkh :> string)))
      /* If the prefix is the animals one, we should hash the `ledgerBasePkh`
         into its animal representation, and check it is the same. */
      | Error(_) => Ok(Animals(prefix))
      };

    let decodeIndex = (index, i) =>
      switch (index->Js.String2.match(Js.Re.fromString("^[0-9]+h$"))) {
      | Some([|_|]) =>
        index
        ->Js.String.substring(~from=0, ~to_=index->Js.String.length - 1)
        ->int_of_string_opt
        ->Result.fromOption(InvalidIndex(i, index))
      | Some(_)
      | None => Error(InvalidIndex(i, index))
      };

    let decodeIndexes = Js.Array.mapi(decodeIndex);

    let fromSecretKey =
        (uri: secretKeyEncoding, ~ledgerBasePkh: PublicKeyHash.t) => {
      let elems = uri->Js.String2.split("/");

      let%Res () =
        elems->Js.Array2.length < 2 ? Error(InvalidEncoding(uri)) : Ok();

      let%Res prefix =
        // The None case is actually impossible, hence the meaningless error
        elems[0]->Result.fromOption(InvalidEncoding(uri));
      let%Res _prefix = prefix->decodePrefix(ledgerBasePkh);

      let%Res scheme =
        // The None case is actually impossible, hence the meaningless error
        elems[1]->Result.fromOption(InvalidEncoding(uri));
      let%Res scheme = scheme->schemeFromString;

      let indexes = elems->Js.Array2.sliceFrom(2)->decodeIndexes;
      let%Res indexes = indexes->Result.collectArray;

      let%ResMap indexes =
        switch (indexes) {
        | [|i1, i2|] => DerivationPath.buildTezosBip44((i1, i2))->Ok
        | p => Error(InvalidPathSize(p))
        };
      {path: indexes, scheme};
    };
  };

  module Encode = {
    let toSecretKey = (t, ~ledgerBasePkh: PublicKeyHash.t): secretKeyEncoding => {
      let (i1, i2) = (t.path :> (int, int));
      let indexes = Format.sprintf("/%dh/%dh", i1, i2);
      Format.sprintf(
        "ledger://%s/%s%s",
        (ledgerBasePkh :> string),
        schemeToString(t.scheme),
        indexes,
      );
    };
  };
};
