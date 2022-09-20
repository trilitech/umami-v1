/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open HDWalletAPI
open HDWalletAPI.Accounts
open HDWalletAPI.Accounts.Scan

let runStreamedScan = (
  ~network,
  ~startIndex=0,
  ~onFoundKey,
  path: DerivationPath.Pattern.t,
  schema,
) =>
  runStream(
    ~network,
    ~startIndex,
    ~onFoundKey=(i, account) => onFoundKey(i, account.publicKeyHash),
    path,
    schema,
    (path, schema) =>
      LedgerAPI.init()->Promise.flatMapOk(tr =>
        LedgerAPI.getKey(~prompt=false, tr, path, schema)->Promise.mapOk(publicKeyHash => {
          publicKeyHash: publicKeyHash,
          encryptedSecretKey: (),
          kind: Regular,
        })
      ),
  )

let importKey = (
  ~config: ConfigContext.env,
  ~name,
  ~index,
  ~derivationPath,
  ~derivationScheme,
  ~ledgerTransport,
  ~ledgerMasterKey,
) => {
  let path = derivationPath->DerivationPath.Pattern.implement(index)
  LedgerAPI.addOrReplaceAlias(
    ~ledgerTransport,
    ~dirpath=config.baseDir(),
    ~alias=name,
    ~path,
    ~scheme=derivationScheme,
    ~ledgerBasePkh=ledgerMasterKey,
  )
}

let importKeys = (
  ~config,
  ~accountsNumber,
  ~startIndex,
  ~basename,
  ~derivationPath,
  ~derivationScheme,
  ~ledgerTransport,
  ~ledgerMasterKey,
) => {
  let rec importKeys = (tr, keys, index) => {
    let name = basename ++ (" /" ++ Int.toString(index))
    index < startIndex + accountsNumber
      ? importKey(
          ~config,
          ~name,
          ~index,
          ~derivationPath,
          ~derivationScheme,
          ~ledgerTransport=tr,
          ~ledgerMasterKey,
        )->Promise.flatMapOk(key => importKeys(tr, list{key, ...keys}, index + 1))
      : List.reverse(keys)->List.toArray->Promise.ok
  }
  importKeys(ledgerTransport, list{}, startIndex)
}

let \"import" = (
  ~config,
  ~timeout=?,
  ~name,
  ~accountsNumber,
  ~derivationPath=DerivationPath.Pattern.fromTezosBip44(DerivationPath.Pattern.default),
  ~derivationScheme=PublicKeyHash.Scheme.ED25519,
  ~ledgerMasterKey,
  (),
) => {
  let tr = LedgerAPI.init(~timeout?, ())
  let addresses = Promise.flatMapOk(tr, tr =>
    importKeys(
      ~config,
      ~accountsNumber,
      ~startIndex=0,
      ~basename=name,
      ~derivationPath,
      ~derivationScheme,
      ~ledgerTransport=tr,
      ~ledgerMasterKey,
    )
  )

  let pEncrypt = Promise.flatMapOk(addresses, _ =>
    name->SecureStorage.Cipher.encrypt("")->Promise.mapOk(registerRecoveryPhrase)
  )

  let p = Promise.flatMapOk2(addresses, pEncrypt, (addresses, ()) => {
    registerSecret(
      ~config,
      ~name,
      ~kind=Secret.Repr.Ledger,
      ~derivationPath,
      ~derivationScheme,
      ~addresses,
      ~masterPublicKey=None,
    )
    Promise.ok()
  })

  p->Promise.flatMapOk(_ => addresses)
}

let derive = (~config, ~timeout=?, ~index, ~alias, ~ledgerMasterKey, ()) => {
  let tr = LedgerAPI.init(~timeout?, ())
  let secret = secretAt(~config, index)->Promise.value

  let address = Promise.flatMapOk2(tr, secret, (tr, secret) =>
    importKey(
      ~config,
      ~name=alias,
      ~index=secret.addresses->Array.length,
      ~derivationPath=secret.derivationPath,
      ~derivationScheme=secret.derivationScheme,
      ~ledgerTransport=tr,
      ~ledgerMasterKey,
    )
  )

  let pUpdate = Promise.flatMapOk2(address, secret, (address, secret) =>
    {...secret, addresses: Array.concat(secret.addresses, [address])}
    ->updateSecretAt(~config, index)
    ->Promise.value
  )

  Promise.flatMapOk2(pUpdate, address, ((), address) => address->Promise.ok)
}

let deriveKeys = (~config, ~timeout=?, ~index, ~accountsNumber, ~ledgerMasterKey, ()) => {
  let secret = secretAt(~config, index)->Promise.value
  let tr = LedgerAPI.init(~timeout?, ())

  let addresses = Promise.flatMapOk2(tr, secret, (tr, secret) =>
    importKeys(
      ~config,
      ~basename=secret.Secret.Repr.name,
      ~startIndex=secret.addresses->Array.length,
      ~accountsNumber,
      ~derivationPath=secret.derivationPath,
      ~derivationScheme=secret.derivationScheme,
      ~ledgerTransport=tr,
      ~ledgerMasterKey,
    )
  )

  let pUpdate = Promise.flatMapOk2(addresses, secret, (addresses, secret) =>
    {...secret, addresses: Array.concat(secret.addresses, addresses)}
    ->updateSecretAt(~config, index)
    ->Promise.value
  )

  Promise.flatMapOk2(pUpdate, addresses, ((), addresses) => addresses->Promise.ok)
}
