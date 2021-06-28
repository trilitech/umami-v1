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

type error =
  | Generic(string)
  | KeyNotFound;

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
  let read: System.Path.t => Future.t(Result.t(t, error));
  let write: (System.Path.t, t) => Future.t(Result.t(unit, error));
  let protect:
    (System.Path.t, unit => Future.t(Result.t(unit, error))) =>
    Future.t(Result.t(unit, error));
  let find: (t, alias => bool) => Result.t(alias, error);
  let filter: (t, alias => bool) => t;
  let remove: (t, key) => t;
  let addOrReplace: (t, key, value) => t;
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
    System.File.read(dirpath / Alias.filename)
    ->Future.mapError(e => Generic(e))
    ->Future.mapOk(parse);

  let write = (dirpath, aliases) =>
    System.File.write(~name=dirpath / Alias.filename, stringify(aliases))
    ->Future.mapError(e => Generic(e));

  let mkTmpCopy = dirpath => {
    let tmpName = !(System.Path.toString(Alias.filename) ++ ".tmp");
    System.File.copy(
      ~name=dirpath / Alias.filename,
      ~dest=dirpath / tmpName,
      ~mode=System.File.CopyMode.copy_ficlone,
    )
    ->Future.mapError(e => Generic(e));
  };

  let restoreTmpCopy = dirpath => {
    let tmpName = !(System.Path.toString(Alias.filename) ++ ".tmp");
    System.File.copy(
      ~name=dirpath / tmpName,
      ~dest=dirpath / Alias.filename,
      ~mode=System.File.CopyMode.copy_ficlone,
    )
    ->Future.mapError(e => Generic(e));
  };

  let rmTmpCopy = dirpath => {
    let tmpName = !(System.Path.toString(Alias.filename) ++ ".tmp");
    System.File.rm(~name=dirpath / tmpName)
    ->Future.mapError(e => Generic(e));
  };

  let protect = (dirpath, f) => {
    dirpath
    ->mkTmpCopy
    ->Future.flatMapOk(_ => f())
    ->Future.flatMap(
        fun
        | Ok () => dirpath->rmTmpCopy
        | Error(e) => dirpath->restoreTmpCopy->Future.map(_ => Error(e)),
      );
  };

  let find = (aliases: t, f) =>
    aliases->Js.Array2.find(f)->ResultEx.fromOption(Error(KeyNotFound));

  let filter = (aliases, f) => aliases->Js.Array2.filter(f);

  let remove = (aliases, aliasName) => {
    aliases->filter(a => a.name != aliasName);
  };

  let addOrReplace = (aliases, alias, value) => {
    let alias = {name: alias, value};
    aliases->remove(alias.name)->Js.Array.concat([|alias|]);
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
  type t = string;
};

module PkhAliases =
  AliasesMaker({
    type t = PkhAlias.t;

    let filename = !"public_key_hashs";
  });

let aliasFromPkh = (~dirpath, ~pkh, ()) => {
  dirpath
  ->PkhAliases.read
  ->Future.flatMapOk(pkhaliases =>
      pkhaliases
      ->PkhAliases.find(a => a.value == pkh)
      ->Result.map(a => a.name)
      ->Future.value
    );
};

let pkFromAlias = (~dirpath, ~alias, ()) => {
  dirpath
  ->PkAliases.read
  ->Future.flatMapOk(pkaliases =>
      pkaliases
      ->PkAliases.find(a => a.name == alias)
      ->Result.map(a => a.value.PkAlias.key)
      ->Future.value
    );
};

let protectAliases = (~dirpath, ~f, ()) => {
  dirpath->PkAliases.protect(_ =>
    dirpath->PkhAliases.protect(_ => dirpath->SecretAliases.protect(f))
  );
};

let updateAlias = (~dirpath, ~update, ()) => {
  let update = () => {
    let aliases =
      /* ensures the three are available */
      PkAliases.read(dirpath)
      ->Future.flatMapOk(pkAliases =>
          PkhAliases.read(dirpath)
          ->Future.flatMapOk(pkhAliases =>
              SecretAliases.read(dirpath)
              ->Future.mapOk(skAliases => (pkAliases, pkhAliases, skAliases))
            )
        );
    let updatedAliases = aliases->Future.mapOk(update);
    updatedAliases->Future.flatMapOk(((pks, pkhs, sks)) =>
      PkAliases.write(dirpath, pks)
      ->Future.flatMapOk(() =>
          PkhAliases.write(dirpath, pkhs)
          ->Future.flatMapOk(() => SecretAliases.write(dirpath, sks))
        )
    );
  };
  protectAliases(~dirpath, ~f=update, ());
};

let addOrReplaceAlias = (~dirpath, ~alias, ~pk, ~pkh, ~sk, ()) => {
  let update = ((pks, pkhs, sks)) => (
    PkAliases.addOrReplace(pks, alias, pk),
    PkhAliases.addOrReplace(pkhs, alias, pkh),
    SecretAliases.addOrReplace(sks, alias, sk),
  );

  updateAlias(~dirpath, ~update, ());
};

let removeAlias = (~dirpath, ~alias, ()) => {
  let update = ((pks, pkhs, sks)) => (
    PkAliases.remove(pks, alias),
    PkhAliases.remove(pkhs, alias),
    SecretAliases.remove(sks, alias),
  );

  updateAlias(~dirpath, ~update, ());
};

type kind =
  | Encrypted
  | Unencrypted
  | Ledger;

let readSecretFromPkh = (address, dirpath) =>
  aliasFromPkh(~dirpath, ~pkh=address, ())
  ->Future.flatMapOk(alias => {
      dirpath
      ->SecretAliases.read
      ->Future.flatMapOk(secretAliases =>
          secretAliases
          ->Js.Array2.find(a => a.SecretAliases.name == alias)
          ->FutureEx.fromOption(~error=KeyNotFound)
          ->Future.mapOk(a => a.SecretAliases.value)
        )
    })
  ->Future.flatMapOk(key =>
      (
        key->Js.String2.startsWith("encrypted:")
          ? Ok((Encrypted, key))
          : key->Js.String2.startsWith("unencrypted:")
              ? Ok((Unencrypted, key))
              : key->Js.String2.startsWith("ledger://")
                  ? Ok((Ledger, key))
                  : Error(Generic("Can't readkey, bad format: " ++ key))
      )
      ->Future.value
    );
