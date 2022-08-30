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
  | NoSecretFound
  | SecretNotFound(int)
  | CannotUpdateSecret(int)
  | RecoveryPhraseNotFound(int)
  | SecretAlreadyImported;

let () =
  Errors.registerHandler(
    "Wallet",
    fun
    | NoSecretFound => I18n.Errors.no_secret_found->Some
    | SecretNotFound(i) => I18n.Errors.secret_not_found(i)->Some
    | CannotUpdateSecret(i) => I18n.Errors.cannot_update_secret(i)->Some
    | RecoveryPhraseNotFound(i) =>
      I18n.Errors.recovery_phrase_not_found(i)->Some
    | SecretAlreadyImported => I18n.Errors.secret_already_imported->Some
    | _ => None,
  );

module Secret = {
  module Repr = Secret;
  type t = Repr.t;

  module Decode = {
    include Json.Decode;

    let kindFromString =
      fun
      | "mnemonics" => Ok(Secret.Mnemonics)
      | "ledger" => Ok(Secret.Ledger)
      | s => Error(s);

    let pathDecoder = json =>
      json
      |> field("derivationScheme", string)
      |> DerivationPath.Pattern.fromString
      |> Result.getExn;

    /* for legacy reasons, the field may not be present. As such, the default
       scheme is ED25519, the one used to derive "classical" accounts */
    let schemeDecoder = json =>
      // field is called derivationCurve for legacy reasons
      (json |> optional(field("derivationCurve", string)))
      ->Option.map(PublicKeyHash.Scheme.fromString)
      ->Option.getWithDefault(Ok(PublicKeyHash.Scheme.ED25519))
      ->Result.getExn;

    /* for legacy reasons, if the field is not present, this implies the account
       is derived froma mnemonic */
    let kindDecoder = json =>
      (json |> optional(field("kind", string)))
      ->Option.map(kindFromString)
      ->Option.getWithDefault(Ok(Secret.Mnemonics))
      ->Result.getExn;

    let addressesDecoder = json =>
      (json |> field("addresses", array(string)))
      ->Array.map(v => v->PublicKeyHash.build->Result.getExn);

    let masterPublicKeyDecoder = json =>
      (json |> optional(field("legacyAddress", string)))
      ->Option.map(v => v->PublicKeyHash.build->Result.getExn);
  };

  let decoder = json =>
    Decode.{
      Repr.name: json |> field("name", string),
      kind: json |> kindDecoder,
      derivationPath: json |> pathDecoder,
      derivationScheme: json |> schemeDecoder,
      addresses: json |> addressesDecoder,
      masterPublicKey: json |> masterPublicKeyDecoder,
    };

  let kindToString =
    fun
    | Secret.Mnemonics => "mnemonics"
    | Ledger => "ledger";

  let encoder = secret =>
    Json.Encode.(
      object_([
        ("name", string(secret.Repr.name)),
        ("kind", string(secret.kind->kindToString)),
        (
          "derivationScheme",
          string(secret.derivationPath->DerivationPath.Pattern.toString),
        ),
        (
          "derivationCurve",
          string(secret.derivationScheme->PublicKeyHash.Scheme.toString),
        ),
        (
          "addresses",
          secret.addresses->Array.map(s => (s :> string))->stringArray,
        ),
        (
          "legacyAddress",
          secret.masterPublicKey->Option.map(pkh => (pkh :> string))
          |> nullable(string),
        ),
      ])
    );
};

module RecoveryPhrasesStorage =
  LocalStorage.Make({
    type t = array(SecureStorage.Cipher.encryptedData);

    let key = "recovery-phrases";

    let encoder = Json.Encode.(array(SecureStorage.Cipher.encoder));
    let decoder = Json.Decode.(array(SecureStorage.Cipher.decoder));
  });

module SecretStorage = {
  include LocalStorage.Make({
    type t = array(Secret.t);

    let key = "secrets";

    let encoder = Json.Encode.(array(Secret.encoder));
    let decoder = Json.Decode.(array(Secret.decoder));
  });

  let set = (~backupFile, storage: t) => {
    let _ =
      backupFile->Option.map(path =>
        BackupFile.make(
          ~derivationPaths=
            storage->Array.keepMap(secret =>
              secret.kind == Mnemonics ? Some(secret.derivationPath) : None
            ),
          ~recoveryPhrases=
            RecoveryPhrasesStorage.get()->Result.getWithDefault([||]),
        )
        ->BackupFile.save(path)
        ->Promise.ignore
      );

    set(storage);
  };
};

module Aliases = {
  type t = array(Alias.t);

  let get = (~config: ConfigContext.env) => {
    let pkhs = config.baseDir()->KeyWallet.PkhAliases.read;
    let sks = config.baseDir()->KeyWallet.SecretAliases.read;

    Promise.flatMapOk2(sks, pkhs, (sks, pkhs) =>
      pkhs
      ->Array.map(({name, value}) => {
          let res = {
            sks
            ->KeyWallet.SecretAliases.find(skAlias => name == skAlias.name)
            ->Result.flatMap(sk =>
                sk.value->KeyWallet.extractPrefixFromSecretKey
              );
          };

          let kind =
            switch (res) {
            | Ok((kind, _)) => Alias.Account(kind)
            | Error(_) => Contact
            };

          Alias.{name, address: value, kind: Some(kind)};
        })
      ->Promise.ok
    );
  };

  let getAliasMap = (~config) => {
    get(~config)
    ->Promise.mapOk(aliases =>
        aliases
        ->Array.map((Alias.{name, address, _}) =>
            ((address :> string), name)
          )
        ->Map.String.fromArray
      );
  };

  let getAliasForAddress = (~config, ~address: PublicKeyHash.t) => {
    getAliasMap(~config)
    ->Promise.mapOk(m => m->Map.String.get((address :> string)));
  };

  let getAddressForAlias = (~config, ~alias) => {
    get(~config)
    ->Promise.mapOk(aliases =>
        aliases
        ->Array.map((Alias.{name, address, _}) => (name, address))
        ->Map.String.fromArray
        ->Map.String.get(alias)
      );
  };

  let add = (~config: ConfigContext.env, ~alias, ~address) =>
    KeyWallet.addOrReplacePkhAlias(
      ~dirpath=config.baseDir(),
      ~alias,
      ~pkh=address,
    );

  let delete = (~config: ConfigContext.env, ~alias) =>
    KeyWallet.removePkhAlias(~dirpath=config.baseDir(), ~alias);

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename = (~config: ConfigContext.env, renaming) =>
    KeyWallet.renameAlias(
      ~dirpath=config.baseDir(),
      ~oldName=renaming.old_name,
      ~newName=renaming.new_name,
    );
};

module Accounts = {
  type t = array(Secret.t);
  type name = string;

  let secrets = (~config as _) => {
    SecretStorage.get()->Result.mapError(_ => NoSecretFound);
  };

  let recoveryPhrases = () => {
    RecoveryPhrasesStorage.get();
  };

  let get = (~config: ConfigContext.env) => {
    let pkhs = config.baseDir()->KeyWallet.PkhAliases.read;
    let sks = config.baseDir()->KeyWallet.SecretAliases.read;

    Promise.flatMapOk2(pkhs, sks, (pkhs, sks) =>
      pkhs
      ->Array.keepMap(({name, value}) => {
          let res = {
            sks
            ->KeyWallet.SecretAliases.find(skAlias => name == skAlias.name)
            ->Result.flatMap(sk =>
                sk.value->KeyWallet.extractPrefixFromSecretKey
              );
          };

          switch (res) {
          | Ok((kind, _)) => Some(Account.{name, address: value, kind})
          | Error(_) => None
          };
        })
      ->Promise.ok
    );
  };

  let secretAt = (~config, index) => {
    secrets(~config)
    ->Result.flatMap(secrets =>
        Result.fromOption(secrets[index], SecretNotFound(index))
      );
  };

  let updateSecretAt = (~config, secret, index) => {
    secrets(~config)
    ->Result.flatMap(secrets =>
        if (secrets[index] = secret) {
          SecretStorage.set(
            ~backupFile=config.ConfigContext.backupFile,
            secrets,
          )
          ->Ok;
        } else {
          Error(CannotUpdateSecret(index));
        }
      );
  };

  let recoveryPhraseAt = (index, ~password) => {
    recoveryPhrases()
    ->Promise.value
    ->Promise.flatMapOk(recoveryPhrases =>
        recoveryPhrases[index]
        ->Promise.fromOption(~error=RecoveryPhraseNotFound(index))
      )
    ->Promise.flatMapOk(data =>
        SecureStorage.Cipher.decrypt2(password, data)
      );
  };

  let importFromSigner =
      (~config: ConfigContext.env, ~alias, ~prefix, ~secretKey, signer) => {
    let pk =
      signer
      ->ReTaquitoSigner.publicKey
      ->Promise.mapOk(KeyWallet.mnemonicPkValue);

    let pkh = signer->ReTaquitoSigner.publicKeyHash;

    Promise.flatMapOk2(
      pk,
      pkh,
      (pk, pkh) => {
        let skUri = KeyWallet.Prefixes.toString(prefix) ++ secretKey;
        KeyWallet.addOrReplaceAlias(
          ~dirpath=config.baseDir(),
          ~alias,
          ~pk,
          ~pkh,
          ~sk=skUri,
        );
      },
    );
  };

  let import = (~config, ~alias, ~prefix, ~secretKey, ~password) => {
    let signer =
      ReTaquitoSigner.MemorySigner.create(
        ~secretKey,
        ~passphrase=password,
        (),
      );

    let pkh = signer->Promise.flatMapOk(ReTaquitoSigner.publicKeyHash);

    Promise.flatMapOk2(pkh, signer, (pkh, signer) =>
      importFromSigner(~config, ~alias, ~prefix, ~secretKey, signer)
      ->Promise.mapOk(() => pkh)
    );
  };

  let derive = (~config: ConfigContext.env, ~index, ~alias, ~password) => {
    let secret = secretAt(~config, index)->Promise.value;
    let recoveryPhrase = recoveryPhraseAt(index, ~password);

    let edesk =
      Promise.flatMapOk2(secret, recoveryPhrase, (secret, recoveryPhrase) =>
        secret.derivationPath
        ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
        ->HD.edesk(recoveryPhrase->HD.seed, ~password)
      );

    let address =
      edesk->Promise.flatMapOk(edesk =>
        import(
          ~config,
          ~alias,
          ~prefix=Encrypted,
          ~secretKey=edesk,
          ~password,
        )
      );

    Promise.flatMapOk2(secret, address, (secret, address) =>
      {...secret, addresses: Array.concat(secret.addresses, [|address|])}
      ->updateSecretAt(~config, index)
      ->Result.map(() => address)
      ->Promise.value
    );
  };

  let unsafeDelete = (~config: ConfigContext.env, name) =>
    KeyWallet.removeAlias(~dirpath=config.baseDir(), ~alias=name);

  let delete = (~config, name) => {
    let address =
      Aliases.getAddressForAlias(~config, ~alias=name)
      ->Promise.flatMapOk(address =>
          unsafeDelete(~config, name)->Promise.mapOk(() => address)
        );

    address->Promise.flatMapOk(address => {
      let secrets = secrets(~config);

      switch (secrets) {
      | Ok(secrets) =>
        let secrets =
          secrets->Array.map(secret =>
            address == secret.masterPublicKey
              ? {...secret, masterPublicKey: None} : secret
          );

        SecretStorage.set(~backupFile=config.backupFile, secrets)->Promise.ok;
      | Error(NoSecretFound) => Promise.ok()
      | Error(e) => e->Promise.err
      };
    });
  };

  let deleteSecretAt = (~config, index) => {
    let secretsBefore = secrets(~config)->Promise.value;
    let aliases = Aliases.getAliasMap(~config);

    let deletedAddresses =
      Promise.flatMapOk2(aliases, secretsBefore, (aliases, secretsBefore) =>
        secretsBefore[index]
        ->Option.map(secret =>
            secret.addresses
            ->Array.concat(
                secret.masterPublicKey
                ->Option.mapWithDefault([||], pkh => [|pkh|]),
              )
            ->Array.keepMap(v => aliases->Map.String.get((v :> string)))
          )
        ->Promise.fromOption(~error=SecretNotFound(index))
      );

    let deleteAddressP =
      deletedAddresses->Promise.flatMapOk(addresses =>
        addresses->Array.reduce(Promise.ok(), (acc, addr) =>
          acc->Promise.flatMapOk(() => addr->unsafeDelete(~config))
        )
      );

    let secretsBeforeP =
      Promise.flatMapOk2(
        secretsBefore,
        deleteAddressP,
        (secretsBefore, ()) => {
          let _ =
            secretsBefore->Js.Array2.spliceInPlace(
              ~pos=index,
              ~remove=1,
              ~add=[||],
            );
          SecretStorage.set(~backupFile=config.backupFile, secretsBefore);
          Promise.ok();
        },
      );

    let applyStorage = () => {
      switch (recoveryPhrases()) {
      | Ok(recoveryPhrases) =>
        let _ =
          recoveryPhrases->Js.Array2.spliceInPlace(
            ~pos=index,
            ~remove=1,
            ~add=[||],
          );
        RecoveryPhrasesStorage.set(recoveryPhrases);
      | Error(_) => ()
      };

      let secretsAfter = secrets(~config);
      switch (secretsAfter) {
      | Ok([||])
      | Error(NoSecretFound) =>
        SecureStorage.LockStorage.remove();
        RecoveryPhrasesStorage.remove();
        SecretStorage.remove();
      | _ => ()
      };
    };

    secretsBeforeP->Promise.flatMapOk(() => {
      applyStorage();
      Promise.ok();
    });
  };

  let legacyImport = (~config, alias, recoveryPhrase, ~password) => {
    HD.edeskLegacy(recoveryPhrase, ~password)
    ->Promise.flatMapOk(secretKey =>
        import(~config, ~alias, ~prefix=Encrypted, ~secretKey, ~password)
      );
  };

  module Scan = {
    type Errors.t +=
      | APIError(string);

    type kind =
      | Regular
      | Legacy;

    type account('sk) = {
      kind,
      publicKeyHash: PublicKeyHash.t,
      encryptedSecretKey: 'sk,
    };

    let used = (network, address) => {
      network->NodeAPI.Accounts.exists(address);
    };

    let runLegacy = (~recoveryPhrase, ~password) => {
      let encryptedSecretKey = HD.edeskLegacy(recoveryPhrase, ~password);

      let signer =
        encryptedSecretKey->Promise.flatMapOk(secretKey =>
          ReTaquitoSigner.MemorySigner.create(
            ~secretKey,
            ~passphrase=password,
            (),
          )
        );

      let publicKeyHash =
        signer->Promise.flatMapOk(ReTaquitoSigner.publicKeyHash);

      Promise.flatMapOk2(
        publicKeyHash, encryptedSecretKey, (publicKeyHash, encryptedSecretKey) =>
        Promise.ok({kind: Legacy, publicKeyHash, encryptedSecretKey})
      );
    };

    let usedAccount = (~network, ~account, ~onFoundKey, ~index) => {
      used(network, account.publicKeyHash)
      ->Promise.flatMapOk(used =>
          used
            ? onFoundKey(index, account)
            : index == 0 ? onFoundKey(index, account) : Promise.ok()
        );
    };

    let runStream =
        (
          ~network: Network.t,
          ~startIndex=0,
          ~onFoundKey,
          path: DerivationPath.Pattern.t,
          schema,
          getKey,
        ) => {
      let rec loop = n => {
        let path = path->DerivationPath.Pattern.implement(n);

        getKey(path, schema)
        ->Promise.flatMapOk(account => {
            let onFoundKey = (n, account) => {
              onFoundKey(n, account);
              loop(n + 1);
            };
            usedAccount(~network, ~account, ~onFoundKey, ~index=n);
          });
      };
      loop(startIndex);
    };

    let runStreamLegacy = (~network, ~recoveryPhrase, ~password, ~onFoundKey) => {
      let onFoundKey = (n, acc) => onFoundKey(n, acc)->Promise.ok;
      runLegacy(~recoveryPhrase, ~password)
      ->Promise.flatMapOk(account =>
          usedAccount(~network, ~account, ~onFoundKey, ~index=-1)
        );
    };

    let getSeedKey = (~recoveryPhrase, ~password, path, _) => {
      let encryptedSecretKey =
        path->HD.edesk(recoveryPhrase->HD.seed, ~password);

      let signer =
        encryptedSecretKey->Promise.flatMapOk(secretKey =>
          ReTaquitoSigner.MemorySigner.create(
            ~secretKey,
            ~passphrase=password,
            (),
          )
        );

      let publicKeyHash =
        signer->Promise.flatMapOk(ReTaquitoSigner.publicKeyHash);

      Promise.flatMapOk2(publicKeyHash, encryptedSecretKey, (pkh, sk) =>
        {publicKeyHash: pkh, encryptedSecretKey: sk, kind: Regular}
        ->Promise.ok
      );
    };

    let runStreamSeed =
        (
          ~network,
          ~startIndex=0,
          ~onFoundKey,
          ~password,
          secret,
          path: DerivationPath.Pattern.t,
        ) => {
      recoveryPhrases()
      ->Promise.value
      ->Promise.flatMapOk(r =>
          switch (r[secret.Secret.Repr.index]) {
          | Some(recoveryPhrase) =>
            recoveryPhrase
            ->SecureStorage.Cipher.decrypt(password)
            ->Promise.flatMapOk(recoveryPhrase =>
                runStream(
                  ~network,
                  ~startIndex,
                  ~onFoundKey,
                  path,
                  PublicKeyHash.Scheme.ED25519,
                  getSeedKey(~recoveryPhrase, ~password),
                )
                ->Promise.mapOk(() => recoveryPhrase)
              )
            ->Promise.flatMapOk(recoveryPhrase =>
                secret.Secret.Repr.secret.masterPublicKey == None
                  ? runStreamLegacy(
                      ~network,
                      ~recoveryPhrase,
                      ~password,
                      ~onFoundKey,
                    )
                  : Promise.ok()
              )
          | None => Promise.ok()
          }
        );
    };

    let rec runOnSeed =
            (
              ~config: ConfigContext.env,
              seed,
              baseName,
              ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                                DerivationPath.Pattern.default,
                              ),
              ~password,
              ~index=0,
              (),
            )
            : Promise.t(array(PublicKeyHash.t)) => {
      let name = baseName ++ " /" ++ index->Js.Int.toString;
      let edesk =
        derivationPath
        ->DerivationPath.Pattern.implement(index)
        ->HD.edesk(seed, ~password);

      let signer =
        edesk->Promise.flatMapOk(secretKey =>
          ReTaquitoSigner.MemorySigner.create(
            ~secretKey,
            ~passphrase=password,
            (),
          )
        );

      let address = signer->Promise.flatMapOk(ReTaquitoSigner.publicKeyHash);

      let isValidated =
        // always include 0'
        index == 0
          ? Promise.ok(true)
          : address->Promise.flatMapOk(addr => config.network->used(addr));

      isValidated->Promise.flatMapOk(isValidated =>
        if (isValidated) {
          Promise.flatMapOk2(signer, edesk, (signer, edesk) =>
            importFromSigner(
              ~config,
              ~prefix=Encrypted,
              ~secretKey=edesk,
              ~alias=name,
              signer,
            )
          )
          ->Promise.flatMapOk(() =>
              runOnSeed(
                ~config,
                seed,
                baseName,
                ~derivationPath,
                ~password,
                ~index=index + 1,
                (),
              )
            )
          ->Promise.flatMapOk2(address, (addresses, address) =>
              Array.concat([|address|], addresses)->Promise.ok
            );
        } else {
          unsafeDelete(~config, name)->Promise.map(_ => Ok([||]));
        }
      );
    };

    let runLegacy = (~config, recoveryPhrase, name, ~password) => {
      legacyImport(~config, name, recoveryPhrase, ~password)
      ->Promise.flatMapOk(addr =>
          config.network->used(addr)->Promise.mapOk(used => (used, addr))
        )
      ->Promise.flatMapOk(((validated, addr)) =>
          validated
            ? Some(addr)->Promise.ok
            : unsafeDelete(~config, name)->Promise.map(_ => Ok(None))
        );
    };

    let run =
        (
          ~config,
          ~recoveryPhrase,
          ~baseName,
          ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                            DerivationPath.Pattern.default,
                          ),
          ~password,
          ~index=0,
          (),
        ) => {
      let addresses =
        runOnSeed(
          ~config,
          recoveryPhrase->HD.seed,
          baseName,
          ~derivationPath,
          ~password,
          ~index,
          (),
        );

      addresses->Promise.flatMapOk(addresses =>
        runLegacy(~config, recoveryPhrase, baseName ++ " legacy", ~password)
        ->Promise.mapOk(legacyAddress => (addresses, legacyAddress))
      );
    };
  };

  let indexOfRecoveryPhrase = (recoveryPhrase, ~password) =>
    recoveryPhrases()
    ->Result.getWithDefault([||])
    ->Array.map(data => SecureStorage.Cipher.decrypt2(password, data))
    ->List.fromArray
    ->Promise.all
    ->Promise.map(List.toArray)
    ->Promise.map(decryptedRecoveryPhrases =>
        decryptedRecoveryPhrases->Array.getBy(decryptedRecoveryPhrase =>
          decryptedRecoveryPhrase == Ok(recoveryPhrase)
        )
      );

  let registerSecret =
      (
        ~config,
        ~name,
        ~kind,
        ~derivationPath,
        ~derivationScheme,
        ~addresses,
        ~masterPublicKey,
      ) => {
    let secret = {
      Secret.Repr.name,
      kind,
      derivationPath,
      derivationScheme,
      addresses,
      masterPublicKey,
    };
    let secrets =
      secrets(~config)
      ->Result.getWithDefault([||])
      ->Array.concat([|secret|]);
    SecretStorage.set(~backupFile=config.ConfigContext.backupFile, secrets);
  };

  let registerRecoveryPhrase = recoveryPhrase =>
    recoveryPhrases()
    ->Result.getWithDefault([||])
    ->(
        recoveryPhrases => {
          let recoveryPhrases =
            Array.concat(recoveryPhrases, [|recoveryPhrase|]);
          RecoveryPhrasesStorage.set(recoveryPhrases);
        }
      );

  let restore =
      (
        ~config: ConfigContext.env,
        ~backupPhrase,
        ~name,
        ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                          DerivationPath.Pattern.default,
                        ),
        ~derivationScheme=PublicKeyHash.Scheme.ED25519,
        ~password,
        (),
      ) => {
    let backupPhraseConcat = backupPhrase->Js.Array2.joinWith(" ");
    let bpLen = backupPhrase->Array.length;

    let initAndCheckP =
      System.Client.initDir(config.baseDir())
      ->Promise.flatMapOk(() => password->SecureStorage.validatePassword)
      ->Promise.flatMapOk(() =>
          (
            if (bpLen->Bip39.Mnemonic.isStandardLength) {
              backupPhrase->Js.Array2.reducei(
                (res, w, i) =>
                  res->Result.flatMap(() =>
                    w->Bip39.included
                      ? Ok() : Bip39.Mnemonic.UnknownWord(w, i)->Error
                  ),
                Ok(),
              );
            } else {
              Bip39.Mnemonic.IncorrectNumberOfWords->Error;
            }
          )
          ->Promise.value
        )
      ->Promise.flatMapOk(() =>
          indexOfRecoveryPhrase(backupPhraseConcat, ~password)
          ->Promise.map(index =>
              switch (index) {
              | Some(_) => SecretAlreadyImported->Error
              | None => Ok()
              }
            )
        );

    initAndCheckP
    ->Promise.flatMapOk(() =>
        Scan.run(
          ~config,
          ~recoveryPhrase=backupPhraseConcat,
          ~baseName=name,
          ~derivationPath,
          ~password,
          (),
        )
      )
    ->Promise.flatMapOk(((addresses, legacyAddress)) =>
        backupPhraseConcat
        ->SecureStorage.Cipher.encrypt(password)
        ->Promise.mapOk(recoveryPhrase =>
            registerRecoveryPhrase(recoveryPhrase)
          )
        ->Promise.mapOk(() =>
            registerSecret(
              ~config,
              ~name,
              ~kind=Secret.Repr.Mnemonics,
              ~derivationPath,
              ~derivationScheme,
              ~addresses,
              ~masterPublicKey=legacyAddress,
            )
          )
      );
  };

  let parseGalleonBackupFile =
      (~config: ConfigContext.env, ~path, ~json, ~password, ()) => {
    let baseName = path->System.Path.baseName(".tezwallet");
    json
    ->Galleon.parse
    ->Promise.flatMapOk(backupFile =>
        backupFile->Galleon.Account.decode(~passphrase=password)
      )
    ->Promise.flatMapOk(accounts =>
        System.Client.initDir(config.baseDir())
        ->Promise.flatMapOk(_ =>
            accounts
            ->Array.mapWithIndex((index, account) => {
                let name = baseName ++ " " ++ index->Js.Int.toString;
                import(
                  ~config,
                  ~alias=name,
                  ~prefix=Galleon,
                  ~secretKey=account.secretKey,
                  ~password,
                );
              })
            ->Promise.allArray
          )
      )
    ->Promise.mapOk(_ => ());
  };

  let parseUmamiBackupFile =
      (~config: ConfigContext.env, ~json, ~password, ()) => {
    json
    ->BackupFile.parse
    ->Promise.flatMapOk(backupFile =>
        Array.zip(backupFile.recoveryPhrases, backupFile.derivationPaths)
        ->Promise.reducei(
            Ok(), (_, (encryptedBackupPhrase, derivationPath), index) =>
            encryptedBackupPhrase
            ->SecureStorage.Cipher.decrypt(password)
            ->Promise.flatMapOk(backupPhrase =>
                restore(
                  ~config,
                  ~backupPhrase=backupPhrase->Js.String2.split(" "),
                  ~name="Secret " ++ index->string_of_int,
                  ~derivationPath,
                  ~password,
                  (),
                )
              )
          )
        ->Promise.tapError(_
            // delete everything if an error occured
            =>
              System.Client.resetDir(config.baseDir())
              ->Promise.tapOk(_ => LocalStorage.clear())
            )
      );
  };

  let restoreFromBackupFile =
      (~config: ConfigContext.env, ~backupFile, ~password, ()) => {
    System.File.read(backupFile)
    ->Promise.flatMapOk(file => {
        let result = JsonEx.parse(file);
        Promise.value(result);
      })
    ->Promise.flatMapOk(json =>
        parseGalleonBackupFile(
          ~config,
          ~path=backupFile,
          ~json,
          ~password,
          (),
        )
        ->Promise.flatMapError(_ =>
            parseUmamiBackupFile(~config, ~json, ~password, ())
          )
      );
  };

  let forceBackup = backupFile =>
    SecretStorage.get()
    ->Result.map(secrets => SecretStorage.set(~backupFile, secrets));

  let importMnemonicKeys = (~config, ~accounts, ~password, ~index, ()) => {
    let importLegacyKey = (basename, encryptedSecret) => {
      import(
        ~config,
        ~alias=basename ++ " legacy",
        ~prefix=Encrypted,
        ~secretKey=encryptedSecret,
        ~password,
      )
      ->Promise.mapOk(pkh => Some(pkh));
    };

    let rec importKeys = (basename, index, (accounts, legacy), pkhs) => {
      let alias = basename ++ " /" ++ index->Js.Int.toString;
      switch (accounts) {
      | [] => (pkhs->List.reverse->List.toArray, legacy)->Promise.ok

      // by construction, there should be only one legacy
      | [Scan.{encryptedSecretKey, kind: Legacy}, ...rem] =>
        importLegacyKey(basename, encryptedSecretKey)
        ->Promise.flatMapOk(legacy =>
            importKeys(basename, index + 1, (rem, legacy), pkhs)
          )

      | [{encryptedSecretKey, kind: Regular}, ...rem] =>
        import(
          ~config,
          ~alias,
          ~prefix=Encrypted,
          ~secretKey=encryptedSecretKey,
          ~password,
        )
        ->Promise.flatMapOk(pkh =>
            importKeys(basename, index + 1, (rem, legacy), [pkh, ...pkhs])
          )
      };
    };

    secretAt(~config, index)
    ->Promise.value
    ->Promise.flatMapOk(secret => {
        importKeys(
          secret.name,
          secret.addresses->Array.length,
          (accounts, None),
          [],
        )
        ->Promise.flatMapOk(((addresses, masterPublicKey)) => {
            let secret = {
              ...secret,
              addresses: Array.concat(secret.addresses, addresses),
              masterPublicKey,
            };

            updateSecretAt(secret, ~config, index)
            ->Result.map(() => (addresses, masterPublicKey))
            ->Promise.value;
          })
      });
  };

  let importCustomAuth =
      (~config: ConfigContext.env, ~pkh, ~pk, infos: ReCustomAuth.infos) => {
    System.Client.initDir(config.baseDir())
    ->Promise.flatMapOk(() => {
        let sk = KeyWallet.CustomAuth.Encode.toSecretKey(infos);
        let pk = KeyWallet.customPkValue(~secretPath=sk, pk);

        KeyWallet.addOrReplaceAlias(
          ~dirpath=config.baseDir(),
          ~alias=(infos.handle :> string),
          ~pk,
          ~pkh,
          ~sk,
        );
      })
    ->Promise.mapOk(() => pkh);
  };

  let getPublicKey = (~config: ConfigContext.env, ~account: Account.t) => {
    KeyWallet.pkFromAlias(~dirpath=config.baseDir(), ~alias=account.name);
  };
};
