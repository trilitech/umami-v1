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

open Let;

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
    let%Await pkhs = config.baseDir()->Wallet.PkhAliases.read;

    let%AwaitMap sks = config.baseDir()->Wallet.SecretAliases.read;
    pkhs->Array.map(({name, value}) => {
      let res = {
        let%Res sk =
          sks->Wallet.SecretAliases.find(skAlias => name == skAlias.name);
        sk.value->Wallet.extractPrefixFromSecretKey;
      };

      let kind =
        switch (res) {
        | Ok((kind, _)) => Alias.Account(kind)
        | Error(_) => Contact
        };

      Alias.{name, address: value, kind: Some(kind)};
    });
  };

  let getAliasMap = (~config) => {
    let%AwaitMap aliases = get(~config);

    aliases
    ->Array.map((Alias.{name, address, _}) => ((address :> string), name))
    ->Map.String.fromArray;
  };

  let getAliasForAddress = (~config, ~address: PublicKeyHash.t) => {
    let%AwaitMap m = getAliasMap(~config);
    m->Map.String.get((address :> string));
  };

  let getAddressForAlias = (~config, ~alias) => {
    let%AwaitMap aliases = get(~config);
    aliases
    ->Array.map((Alias.{name, address, _}) => (name, address))
    ->Map.String.fromArray
    ->Map.String.get(alias);
  };

  let add = (~config: ConfigContext.env, ~alias, ~address) =>
    Wallet.addOrReplacePkhAlias(
      ~dirpath=config.baseDir(),
      ~alias,
      ~pkh=address,
    );

  let delete = (~config: ConfigContext.env, ~alias) =>
    Wallet.removePkhAlias(~dirpath=config.baseDir(), ~alias);

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename = (~config: ConfigContext.env, renaming) =>
    Wallet.renameAlias(
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
    let%Await pkhs = config.baseDir()->Wallet.PkhAliases.read;

    let%AwaitMap sks = config.baseDir()->Wallet.SecretAliases.read;

    pkhs->Array.keepMap(({name, value}) => {
      let res = {
        let%Res sk =
          sks->Wallet.SecretAliases.find(skAlias => name == skAlias.name);
        sk.value->Wallet.extractPrefixFromSecretKey;
      };

      switch (res) {
      | Ok((kind, _)) => Some(Account.{name, address: value, kind})
      | Error(_) => None
      };
    });
  };

  let secretAt = (~config, index) => {
    let%Res secrets = secrets(~config);

    Result.fromOption(secrets[index], SecretNotFound(index));
  };

  let updateSecretAt = (~config, secret, index) => {
    let%Res secrets = secrets(~config);

    if (secrets[index] = secret) {
      SecretStorage.set(~backupFile=config.ConfigContext.backupFile, secrets)
      ->Ok;
    } else {
      Error(CannotUpdateSecret(index));
    };
  };

  let recoveryPhraseAt = (index, ~password) => {
    let%Await recoveryPhrases = recoveryPhrases()->Promise.value;
    let%Await data =
      recoveryPhrases[index]
      ->Promise.fromOption(~error=RecoveryPhraseNotFound(index));

    SecureStorage.Cipher.decrypt2(password, data);
  };

  let importFromSigner =
      (~config: ConfigContext.env, ~alias, ~secretKey, signer) => {
    let%Await pk = signer->ReTaquitoSigner.publicKey;
    let pk = Wallet.mnemonicPkValue(pk);
    let%Await pkh = signer->ReTaquitoSigner.publicKeyHash;
    let skUri = Wallet.Prefixes.toString(Encrypted) ++ secretKey;
    Wallet.addOrReplaceAlias(
      ~dirpath=config.baseDir(),
      ~alias,
      ~pk,
      ~pkh,
      ~sk=skUri,
    );
  };

  let import = (~config, ~alias, ~secretKey, ~password) => {
    let%Await signer =
      ReTaquitoSigner.MemorySigner.create(
        ~secretKey,
        ~passphrase=password,
        (),
      );
    let%Await pkh = signer->ReTaquitoSigner.publicKeyHash;
    let%AwaitMap () = importFromSigner(~config, ~alias, ~secretKey, signer);
    pkh;
  };

  let derive = (~config: ConfigContext.env, ~index, ~alias, ~password) => {
    let%Await secret = secretAt(~config, index)->Promise.value;

    let%Await recoveryPhrase = recoveryPhraseAt(index, ~password);

    let%Await edesk =
      secret.derivationPath
      ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
      ->HD.edesk(recoveryPhrase->HD.seed, ~password);

    let%Await address = import(~config, ~secretKey=edesk, ~alias, ~password);

    let%Await () =
      {...secret, addresses: Array.concat(secret.addresses, [|address|])}
      ->updateSecretAt(~config, index)
      ->Promise.value;

    address->Promise.ok;
  };

  let unsafeDelete = (~config: ConfigContext.env, name) =>
    Wallet.removeAlias(~dirpath=config.baseDir(), ~alias=name);

  let delete = (~config, name) => {
    let%Await address = Aliases.getAddressForAlias(~config, ~alias=name);
    let%Await () = unsafeDelete(~config, name);

    let%Ft secrets = secrets(~config)->Promise.value;

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
  };

  let deleteSecretAt = (~config, index) => {
    let%Await secretsBefore = secrets(~config)->Promise.value;
    let%Await aliases = Aliases.getAliasMap(~config);

    let%Await deletedAddresses =
      secretsBefore[index]
      ->Option.map(secret =>
          secret.addresses
          ->Array.concat(
              secret.masterPublicKey
              ->Option.mapWithDefault([||], pkh => [|pkh|]),
            )
          ->Array.keepMap(v => aliases->Map.String.get((v :> string)))
        )
      ->Promise.fromOption(~error=SecretNotFound(index));

    let%AwaitMap () =
      deletedAddresses->Array.reduce(Promise.ok(), (acc, addr) =>
        acc->Promise.flatMapOk(() => addr->unsafeDelete(~config))
      );

    let _ =
      secretsBefore->Js.Array2.spliceInPlace(
        ~pos=index,
        ~remove=1,
        ~add=[||],
      );
    SecretStorage.set(~backupFile=config.backupFile, secretsBefore);

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

  let legacyImport = (~config, alias, recoveryPhrase, ~password) => {
    let%Await secretKey = HD.edeskLegacy(recoveryPhrase, ~password);

    import(~config, ~alias, ~secretKey, ~password);
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

    let used = (config, address) => {
      config->NodeAPI.Accounts.exists(address);
    };

    let runLegacy = (~recoveryPhrase, ~password) => {
      let%Await encryptedSecretKey =
        HD.edeskLegacy(recoveryPhrase, ~password);

      let%Await signer =
        ReTaquitoSigner.MemorySigner.create(
          ~secretKey=encryptedSecretKey,
          ~passphrase=password,
          (),
        );

      let%Await publicKeyHash = signer->ReTaquitoSigner.publicKeyHash;

      Promise.ok({kind: Legacy, publicKeyHash, encryptedSecretKey});
    };

    let usedAccount = (~config, ~account, ~onFoundKey, ~index) => {
      let%Await used = used(config, account.publicKeyHash);

      used
        ? onFoundKey(index, account)
        : index == 0 ? onFoundKey(index, account) : Promise.ok();
    };

    let runStream =
        (
          ~config,
          ~startIndex=0,
          ~onFoundKey,
          path: DerivationPath.Pattern.t,
          schema,
          getKey,
        ) => {
      let rec loop = n => {
        let path = path->DerivationPath.Pattern.implement(n);

        let%Await account = getKey(path, schema);
        let onFoundKey = (n, account) => {
          onFoundKey(n, account);
          loop(n + 1);
        };
        usedAccount(~config, ~account, ~onFoundKey, ~index=n);
      };
      loop(startIndex);
    };

    let runStreamLedger =
        (
          ~config,
          ~startIndex=0,
          ~onFoundKey,
          path: DerivationPath.Pattern.t,
          schema,
        ) =>
      runStream(
        ~config,
        ~startIndex,
        ~onFoundKey=(i, account) => onFoundKey(i, account.publicKeyHash),
        path,
        schema,
        (path, schema) => {
          let%Await tr = LedgerAPI.init();
          let%AwaitMap publicKeyHash =
            LedgerAPI.getKey(~prompt=false, tr, path, schema);
          {publicKeyHash, encryptedSecretKey: (), kind: Regular};
        },
      );

    let runStreamLegacy = (~config, ~recoveryPhrase, ~password, ~onFoundKey) => {
      let onFoundKey = (n, acc) => onFoundKey(n, acc)->Promise.ok;
      let%Await account = runLegacy(~recoveryPhrase, ~password);
      usedAccount(~config, ~account, ~onFoundKey, ~index=-1);
    };

    let getSeedKey = (~recoveryPhrase, ~password, path, _) => {
      let%Await encryptedSecretKey =
        path->HD.edesk(recoveryPhrase->HD.seed, ~password);

      let%Await signer =
        ReTaquitoSigner.MemorySigner.create(
          ~secretKey=encryptedSecretKey,
          ~passphrase=password,
          (),
        );

      let%AwaitMap publicKeyHash = signer->ReTaquitoSigner.publicKeyHash;

      {publicKeyHash, encryptedSecretKey, kind: Regular};
    };

    let runStreamSeed =
        (
          ~config,
          ~startIndex=0,
          ~onFoundKey,
          ~password,
          secret,
          path: DerivationPath.Pattern.t,
        ) => {
      let%Await r = recoveryPhrases()->Promise.value;

      switch (r[secret.Secret.Repr.index]) {
      | Some(recoveryPhrase) =>
        let onFoundKey = (n, acc) => onFoundKey(n, acc);

        let%Await recoveryPhrase =
          recoveryPhrase->SecureStorage.Cipher.decrypt(password);

        let%Await () =
          runStream(
            ~config,
            ~startIndex,
            ~onFoundKey,
            path,
            PublicKeyHash.Scheme.ED25519,
            getSeedKey(~recoveryPhrase, ~password),
          );

        secret.Secret.Repr.secret.masterPublicKey == None
          ? runStreamLegacy(~config, ~recoveryPhrase, ~password, ~onFoundKey)
          : Promise.ok();
      | None => Promise.ok()
      };
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
            ) => {
      let name = baseName ++ " /" ++ index->Js.Int.toString;
      let%Await edesk =
        derivationPath
        ->DerivationPath.Pattern.implement(index)
        ->HD.edesk(seed, ~password);

      let%Await signer =
        ReTaquitoSigner.MemorySigner.create(
          ~secretKey=edesk,
          ~passphrase=password,
          (),
        );

      let%Await address = signer->ReTaquitoSigner.publicKeyHash;

      let%Await isValidated =
        // always include 0'
        index == 0 ? Promise.ok(true) : config->used(address);

      if (isValidated) {
        let%Await () =
          importFromSigner(~config, ~secretKey=edesk, ~alias=name, signer);

        let%AwaitMap addresses =
          runOnSeed(
            ~config,
            seed,
            baseName,
            ~derivationPath,
            ~password,
            ~index=index + 1,
            (),
          );
        Array.concat([|address|], addresses);
      } else {
        unsafeDelete(~config, name)->Promise.map(_ => Ok([||]));
      };
    };

    let runLegacy = (~config, recoveryPhrase, name, ~password) => {
      let%Await legacyAddress =
        legacyImport(~config, name, recoveryPhrase, ~password);

      let%Await isValidated = config->used(legacyAddress);

      isValidated
        ? Some(legacyAddress)->Promise.ok
        : unsafeDelete(~config, name)->Promise.map(_ => Ok(None));
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
      let%Await addresses =
        runOnSeed(
          ~config,
          recoveryPhrase->HD.seed,
          baseName,
          ~derivationPath,
          ~password,
          ~index,
          (),
        );
      let%AwaitMap legacyAddresses =
        runLegacy(~config, recoveryPhrase, baseName ++ " legacy", ~password);
      (addresses, legacyAddresses);
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
    let%Await () = System.Client.initDir(config.baseDir());
    let backupPhraseConcat = backupPhrase->Js.Array2.joinWith(" ");

    let%Await () = password->SecureStorage.validatePassword;

    let bpLen = backupPhrase->Array.length;

    let%Await () =
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
      ->Promise.value;

    let%Await () =
      indexOfRecoveryPhrase(backupPhraseConcat, ~password)
      ->Promise.map(index =>
          switch (index) {
          | Some(_) => SecretAlreadyImported->Error
          | None => Ok()
          }
        );

    let%Await (addresses, legacyAddress) =
      Scan.run(
        ~config,
        ~recoveryPhrase=backupPhraseConcat,
        ~baseName=name,
        ~derivationPath,
        ~password,
        (),
      );

    let%AwaitMap () =
      backupPhraseConcat
      ->SecureStorage.Cipher.encrypt(password)
      ->Promise.mapOk(recoveryPhrase =>
          registerRecoveryPhrase(recoveryPhrase)
        );

    registerSecret(
      ~config,
      ~name,
      ~kind=Secret.Repr.Mnemonics,
      ~derivationPath,
      ~derivationScheme,
      ~addresses,
      ~masterPublicKey=legacyAddress,
    );
  };

  let restoreFromBackupFile =
      (~config: ConfigContext.env, ~backupFile, ~password, ()) => {
    let%Await backupFile = BackupFile.read(backupFile);
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
        );
  };

  let forceBackup = backupFile =>
    SecretStorage.get()
    ->Result.map(secrets => SecretStorage.set(~backupFile, secrets));

  let importMnemonicKeys = (~config, ~accounts, ~password, ~index, ()) => {
    let importLegacyKey = (basename, encryptedSecret) => {
      let%AwaitMap pkh =
        import(
          ~config,
          ~alias=basename ++ " legacy",
          ~secretKey=encryptedSecret,
          ~password,
        );
      Some(pkh);
    };

    let rec importKeys = (basename, index, (accounts, legacy), pkhs) => {
      let alias = basename ++ " /" ++ index->Js.Int.toString;
      switch (accounts) {
      | [] => (pkhs->List.reverse->List.toArray, legacy)->Promise.ok

      // by construction, there should be only one legacy
      | [Scan.{encryptedSecretKey, kind: Legacy}, ...rem] =>
        let%Await legacy = importLegacyKey(basename, encryptedSecretKey);
        importKeys(basename, index + 1, (rem, legacy), pkhs);

      | [{encryptedSecretKey, kind: Regular}, ...rem] =>
        let%Await pkh =
          import(~config, ~alias, ~secretKey=encryptedSecretKey, ~password);
        importKeys(basename, index + 1, (rem, legacy), [pkh, ...pkhs]);
      };
    };

    let%Await secret = secretAt(~config, index)->Promise.value;

    let%AwaitRes (addresses, masterPublicKey) =
      importKeys(
        secret.name,
        secret.addresses->Array.length,
        (accounts, None),
        [],
      );

    let secret = {
      ...secret,
      addresses: Array.concat(secret.addresses, addresses),
      masterPublicKey,
    };

    let%ResMap () = updateSecretAt(secret, ~config, index);
    (addresses, masterPublicKey);
  };

  let importLedgerKey =
      (
        ~config: ConfigContext.env,
        ~name,
        ~index,
        ~derivationPath,
        ~derivationScheme,
        ~ledgerTransport,
        ~ledgerMasterKey,
      ) => {
    let path = derivationPath->DerivationPath.Pattern.implement(index);
    LedgerAPI.addOrReplaceAlias(
      ~ledgerTransport,
      ~dirpath=config.baseDir(),
      ~alias=name,
      ~path,
      ~scheme=derivationScheme,
      ~ledgerBasePkh=ledgerMasterKey,
    );
  };

  let importLedgerKeys =
      (
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
      let name = basename ++ " /" ++ Int.toString(index);
      index < startIndex + accountsNumber
        ? importLedgerKey(
            ~config,
            ~name,
            ~index,
            ~derivationPath,
            ~derivationScheme,
            ~ledgerTransport=tr,
            ~ledgerMasterKey,
          )
          ->Promise.flatMapOk(key =>
              importKeys(tr, [key, ...keys], index + 1)
            )
        : List.reverse(keys)->List.toArray->Promise.ok;
    };
    importKeys(ledgerTransport, [], startIndex);
  };

  let importLedger =
      (
        ~config,
        ~timeout=?,
        ~name,
        ~accountsNumber,
        ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                          DerivationPath.Pattern.default,
                        ),
        ~derivationScheme=PublicKeyHash.Scheme.ED25519,
        ~ledgerMasterKey,
        (),
      ) => {
    let%Await tr = LedgerAPI.init(~timeout?, ());
    let%Await addresses =
      importLedgerKeys(
        ~config,
        ~accountsNumber,
        ~startIndex=0,
        ~basename=name,
        ~derivationPath,
        ~derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
      );

    let%AwaitMap () =
      name
      ->SecureStorage.Cipher.encrypt("")
      ->Promise.mapOk(recoveryPhrase =>
          registerRecoveryPhrase(recoveryPhrase)
        );

    registerSecret(
      ~config,
      ~name,
      ~kind=Secret.Repr.Ledger,
      ~derivationPath,
      ~derivationScheme,
      ~addresses,
      ~masterPublicKey=None,
    );

    addresses;
  };

  let importCustomAuth =
      (~config: ConfigContext.env, ~pkh, ~pk, infos: ReCustomAuth.infos) => {
    let%Await () = System.Client.initDir(config.baseDir());

    let sk = Wallet.CustomAuth.Encode.toSecretKey(infos);
    let pk = Wallet.customPkValue(~secretPath=sk, pk);

    let%AwaitMap () =
      Wallet.addOrReplaceAlias(
        ~dirpath=config.baseDir(),
        ~alias=(infos.handle :> string),
        ~pk,
        ~pkh,
        ~sk,
      );
    pkh;
  };

  let deriveLedger =
      (~config, ~timeout=?, ~index, ~alias, ~ledgerMasterKey, ()) => {
    let%Await secret = secretAt(~config, index)->Promise.value;
    let%Await tr = LedgerAPI.init(~timeout?, ());

    let%Await address =
      importLedgerKey(
        ~config,
        ~name=alias,
        ~index=secret.addresses->Array.length,
        ~derivationPath=secret.derivationPath,
        ~derivationScheme=secret.derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
      );

    let%Await () =
      {...secret, addresses: Array.concat(secret.addresses, [|address|])}
      ->updateSecretAt(~config, index)
      ->Promise.value;

    address->Promise.ok;
  };

  let deriveLedgerKeys =
      (~config, ~timeout=?, ~index, ~accountsNumber, ~ledgerMasterKey, ()) => {
    let%Await secret = secretAt(~config, index)->Promise.value;
    let%Await tr = LedgerAPI.init(~timeout?, ());

    let%Await addresses =
      importLedgerKeys(
        ~config,
        ~basename=secret.Secret.Repr.name,
        ~startIndex=secret.addresses->Array.length,
        ~accountsNumber,
        ~derivationPath=secret.derivationPath,
        ~derivationScheme=secret.derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
      );

    let%Await () =
      {...secret, addresses: Array.concat(secret.addresses, addresses)}
      ->updateSecretAt(~config, index)
      ->Promise.value;

    addresses->Promise.ok;
  };

  let getPublicKey = (~config: ConfigContext.env, ~account: Account.t) => {
    Wallet.pkFromAlias(~dirpath=config.baseDir(), ~alias=account.name);
  };
};
