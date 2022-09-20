/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com>          */
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


module Make = (S: KeyWalletIntf.SYSTEM): (KeyWalletIntf.WALLET with module Sys = S) => {
  module Sys = S

  open Sys.Path.Ops

  type Errors.t +=
    | KeyNotFound
    | KeyBadFormat(string)
    | InvalidEncoding(string)

  let () = Errors.registerHandler("Wallet", x =>
    switch x {
    | KeyNotFound => I18n.Wallet.key_not_found->Some
    | KeyBadFormat(s) => I18n.Wallet.key_bad_format(s)->Some
    | InvalidEncoding(e) => I18n.Wallet.invalid_encoding(e)->Some
    | _ => None
    }
  )

  module type AliasType = {
    type t
    let filename: Sys.Path.t
  }

  module type AliasesMakerType = KeyWalletIntf.AliasesMakerType with type path := Sys.Path.t

  module AliasesMaker = (Alias: AliasType): (
    AliasesMakerType with type key := string and type value := Alias.t
  ) => {
    type alias = {
      name: string,
      value: Alias.t,
    }

    type t = array<alias>

    @val @scope("JSON") external parse: string => t = "parse"
    @val @scope("JSON") external stringify: t => string = "stringify"

    let read = dirpath => Sys.File.read(dirpath / Alias.filename)->Promise.mapOk(parse)

    let write = (dirpath, aliases) =>
      Sys.File.write(~name=dirpath / Alias.filename, stringify(aliases))

    let protect = (dirpath, f) => Sys.File.protect(~name=dirpath / Alias.filename, ~transaction=f)

    let find = (aliases: t, f) => aliases->Js.Array2.find(f)->Result.fromOption(KeyNotFound)

    let filter = (aliases, f) => aliases->Js.Array2.filter(f)

    let remove = (aliases, aliasName) => aliases->filter(a => a.name != aliasName)

    let addOrReplace = (aliases, alias, value) => {
      let alias = {name: alias, value: value}
      aliases->remove(alias.name)->Js.Array.concat([alias])
    }

    let rename = (aliases, ~oldName, ~newName) =>
      aliases->Js.Array2.map(alias => alias.name == oldName ? {...alias, name: newName} : alias)
  }

  module SecretAlias = {
    type t = string
  }

  module SecretAliases = AliasesMaker({
    type t = SecretAlias.t
    let filename = !"secret_keys"
  })

  module PkAlias = {
    type t = {
      locator: string,
      key: string,
    }
  }

  module PkAliases = AliasesMaker({
    type t = PkAlias.t

    let filename = !"public_keys"
  })

  module PkhAlias = {
    type t = PublicKeyHash.t
  }

  module PkhAliases = AliasesMaker({
    type t = PkhAlias.t

    let filename = !"public_key_hashs"
  })

  let aliasFromPkh = (~dirpath, ~pkh) =>
    dirpath
    ->PkhAliases.read
    ->Promise.flatMapOk(pkhaliases =>
      pkhaliases->PkhAliases.find(a => a.value == pkh)->Result.map(a => a.name)->Promise.value
    )

  let pkFromAlias = (~dirpath, ~alias) =>
    dirpath
    ->PkAliases.read
    ->Promise.flatMapOk(pkaliases => pkaliases->PkAliases.find(a => a.name == alias)->Promise.value)
    ->Promise.mapOk(a => a.value.PkAlias.key)

  let updatePkhAlias = (~dirpath, ~update) =>
    dirpath->PkhAliases.protect(_ =>
      PkhAliases.read(dirpath)->Promise.flatMapOk(pkhAliases => {
        let pkhAliases = update(pkhAliases)
        dirpath->PkhAliases.write(pkhAliases)
      })
    )

  let addOrReplacePkhAlias = (~dirpath, ~alias, ~pkh) =>
    updatePkhAlias(~dirpath, ~update=pkhs => PkhAliases.addOrReplace(pkhs, alias, pkh))

  let removePkhAlias = (~dirpath, ~alias) =>
    updatePkhAlias(~dirpath, ~update=pkhs => PkhAliases.remove(pkhs, alias))

  let renamePkhAlias = (~dirpath, ~oldName, ~newName) =>
    updatePkhAlias(~dirpath, ~update=pkhs => PkhAliases.rename(pkhs, ~oldName, ~newName))

  let protectAliases = (~dirpath, ~f) =>
    dirpath->PkAliases.protect(_ =>
      dirpath->PkhAliases.protect(_ => dirpath->SecretAliases.protect(f))
    )

  let updateAlias = (~dirpath, ~update) => {
    let update = () =>
      PkAliases.read(dirpath)
      ->Promise.flatMapOk(pkAliases =>
        PkhAliases.read(dirpath)->Promise.flatMapOk(pkhAliases =>
          SecretAliases.read(dirpath)->Promise.mapOk(skAliases => (
            pkAliases,
            pkhAliases,
            skAliases,
          ))
        )
      )
      ->Promise.mapOk(((pkAliases, pkhAliases, skAliases)) =>
        update(pkAliases, pkhAliases, skAliases)
      )
      ->Promise.flatMapOk(((pks, pkhs, sks)) =>
        PkAliases.write(dirpath, pks)
        ->Promise.flatMapOk(() => PkhAliases.write(dirpath, pkhs))
        ->Promise.flatMapOk(() => SecretAliases.write(dirpath, sks))
      )
    protectAliases(~dirpath, ~f=update)
  }

  let addOrReplaceAlias = (~dirpath, ~alias, ~pk, ~pkh, ~sk) => {
    let update = (pks, pkhs, sks) => (
      PkAliases.addOrReplace(pks, alias, pk),
      PkhAliases.addOrReplace(pkhs, alias, pkh),
      SecretAliases.addOrReplace(sks, alias, sk),
    )

    updateAlias(~dirpath, ~update)
  }

  let removeAlias = (~dirpath, ~alias) => {
    let update = (pks, pkhs, sks) => (
      PkAliases.remove(pks, alias),
      PkhAliases.remove(pkhs, alias),
      SecretAliases.remove(sks, alias),
    )

    updateAlias(~dirpath, ~update)
  }

  let renameAlias = (~dirpath, ~oldName, ~newName) => {
    let update = (pks, pkhs, sks) => (
      PkAliases.rename(pks, ~oldName, ~newName),
      PkhAliases.rename(pkhs, ~oldName, ~newName),
      SecretAliases.rename(sks, ~oldName, ~newName),
    )

    updateAlias(~dirpath, ~update)
  }

  module CustomAuth = {
    module Decode = {
      let fromSecretKey = uri => {
        let elems = uri->Js.String2.split("/")

        let r = switch elems {
        | [provider, handle] =>
          provider
          ->ReCustomAuthType.providerFromString
          ->Result.map(provider => (provider, handle->ReCustomAuthType.Handle.fromString))

        | _ => Error(InvalidEncoding(uri))
        }

        r->Result.map(((provider, handle)) => {
          open ReCustomAuth
          {provider: provider, handle: handle}
        })
      }
    }

    module Encode = {
      open ReCustomAuth
      let toSecretKey = t =>
        Format.sprintf(
          "customauth://%s/%s",
          t.provider->ReCustomAuth.providerToString,
          (t.handle :> string),
        )
    }
  }

  type kind = Account.kind =
    | Encrypted
    | Unencrypted
    | Ledger
    | Galleon
    | CustomAuth(ReCustomAuth.infos)

  module Prefixes = {
    type t =
      | Encrypted
      | Unencrypted
      | Ledger
      | Galleon
      | CustomAuth

    let fromKind = x =>
      switch x {
      | Account.Encrypted => Encrypted
      | Unencrypted => Unencrypted
      | Ledger => Ledger
      | Galleon => Galleon
      | CustomAuth(_) => CustomAuth
      }

    let toString = x =>
      switch x {
      | Encrypted => "encrypted:"
      | Unencrypted => "unencrypted:"
      | Ledger => "ledger://"
      | Galleon => "galleon:"
      | CustomAuth => "customauth://"
      }
  }

  let extractPrefixFromSecretKey = k => {
    let sub = (k, kind) =>
      k->Js.String2.substringToEnd(~from=String.length(kind->Prefixes.toString))

    let checkStart = (k, pref) => k->Js.String2.startsWith(Prefixes.toString(pref))

    let buildRes = pref => Ok((pref, k->sub(pref->Prefixes.fromKind)))

    switch k {
    | k if checkStart(k, Prefixes.Encrypted) => buildRes(Encrypted)
    | k if checkStart(k, Prefixes.Unencrypted) => buildRes(Unencrypted)
    | k if checkStart(k, Prefixes.Ledger) => buildRes(Ledger)
    | k if checkStart(k, Prefixes.Galleon) => buildRes(Galleon)
    | k if checkStart(k, Prefixes.CustomAuth) =>
      let uri = sub(k, Prefixes.CustomAuth)
      CustomAuth.Decode.fromSecretKey(uri)->Result.map(infos => (CustomAuth(infos), uri))
    | k => Error(KeyBadFormat(k))
    }
  }

  let readSecretFromPkh = (address, dirpath) => {
    let alias = aliasFromPkh(~dirpath, ~pkh=address)
    let secretAliases = dirpath->SecretAliases.read

    Promise.flatMapOk2(alias, secretAliases, (alias, secretAliases) =>
      secretAliases
      ->Js.Array2.find(a => a.SecretAliases.name == alias)
      ->Promise.fromOption(~error=KeyNotFound)
      ->Promise.flatMapOk(({value: k}) => extractPrefixFromSecretKey(k)->Promise.value)
    )
  }

  let mnemonicPkValue = pk => {
    open PkAlias
    {locator: "unencrypted:" ++ pk, key: pk}
  }

  let customPkValue = (~secretPath, pk) => {
    open PkAlias
    {locator: secretPath, key: pk}
  }

  module Ledger = {
    type Errors.t +=
      | InvalidPathSize(array<int>)
      | InvalidIndex(int, string)
      | InvalidLedger(string)

    let () = Errors.registerHandler("Ledger", x =>
      switch x {
      | InvalidPathSize(p) => I18n.Wallet.invalid_path_size(p->Js.String.make)->Some
      | InvalidIndex(index, value) => I18n.Wallet.invalid_index(index, value)->Some
      | InvalidLedger(p) => I18n.Wallet.invalid_ledger(p)->Some
      | _ => None
      }
    )

    @ocaml.doc(" Format of a ledger encoded secret key: `ledger://prefix/scheme/path`, where:
    - prefix is the public key derivated from the path `44'/1729'` and scheme
     `ed25519`, as a public key hash or encoded using the \"crouching tiger\"
      algorithm
    - scheme is either `ed25519`, `secp256k1` or `P-256`
    - path are the indexes after `44'/1279'` in a Tezos derivation path, where
      `'` has been substituted to `h`.
  ")
    type secretKeyEncoding = string

    type prefix =
      | Animals(string)
      | Pkh(string)

    type scheme = PublicKeyHash.Scheme.t = ED25519 | SECP256K1 | P256

    type t = {
      path: DerivationPath.tezosBip44,
      scheme: scheme,
    }

    type masterKey = PublicKeyHash.t
    let masterKeyPath = DerivationPath.build([44, 1729])
    let masterKeyScheme = ED25519

    module Decode = {
      let decodePrefix = (prefix, ledgerBasePkh: PublicKeyHash.t) =>
        switch PublicKeyHash.build(prefix) {
        | Ok(pkh) =>
          pkh == ledgerBasePkh ? Ok(Pkh(prefix)) : Error(InvalidLedger((ledgerBasePkh :> string)))
        /* If the prefix is the animals one, we should hash the `ledgerBasePkh`
         into its animal representation, and check it is the same. */
        | Error(_) => Ok(Animals(prefix))
        }

      let decodeIndex = (index, i) =>
        switch index->Js.String2.match_(Js.Re.fromString("^[0-9]+h$")) {
        | Some([_]) =>
          index
          ->Js.String.substring(~from=0, ~to_=index->Js.String.length - 1)
          ->int_of_string_opt
          ->Result.fromOption(InvalidIndex(i, index))
        | Some(_)
        | None =>
          Error(InvalidIndex(i, index))
        }

      let decodeIndexes = Js.Array.mapi(decodeIndex)

      let fromSecretKey = (uri: secretKeyEncoding, ~ledgerBasePkh: PublicKeyHash.t) => {
        let elems = uri->Js.String2.split("/")

        let r = elems->Js.Array2.length < 2 ? Error(InvalidEncoding(uri)) : Ok()

        r->Result.flatMap(() => {
          let prefix = // The None case is actually impossible, hence the meaningless error
          elems[0]->Result.fromOption(InvalidEncoding(uri))
          let r = prefix->Result.flatMap(prefix => prefix->decodePrefix(ledgerBasePkh))

          r->Result.flatMap(_ => {
            let scheme = // The None case is actually impossible, hence the meaningless error
            elems[1]->Result.fromOption(InvalidEncoding(uri))
            let scheme = scheme->Result.flatMap(PublicKeyHash.Scheme.fromString)

            let indexes = elems->Js.Array2.sliceFrom(2)->decodeIndexes
            let indexes = indexes->Result.collectArray

            let indexes = indexes->Result.flatMap(indexes =>
              switch indexes {
              | [i1, i2] => DerivationPath.buildTezosBip44((i1, i2))->Ok
              | p => Error(InvalidPathSize(p))
              }
            )

            Result.map2(indexes, scheme, (indexes, scheme) => {path: indexes, scheme: scheme})
          })
        })
      }
    }

    module Encode = {
      let toSecretKey = (t, ~ledgerBasePkh: PublicKeyHash.t): secretKeyEncoding => {
        let (i1, i2) = (t.path :> (int, int))
        let indexes = Format.sprintf("/%dh/%dh", i1, i2)
        Format.sprintf(
          "ledger://%s/%s%s",
          (ledgerBasePkh :> string),
          PublicKeyHash.Scheme.toString(t.scheme),
          indexes,
        )
      }
    }
  }
}
