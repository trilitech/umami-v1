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
      ->Option.map(Wallet.Ledger.schemeFromString)
      ->Option.getWithDefault(Ok(Wallet.Ledger.ED25519))
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
          string(secret.derivationScheme->Wallet.Ledger.schemeToString),
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

module Aliases = {
  type t = array((string, PublicKeyHash.t));

  let get = (~config) => {
    let%FResMap addresses =
      config
      ->ConfigUtils.baseDir
      ->Wallet.PkhAliases.read
      ->Future.mapError(e => e->ErrorHandler.Wallet);

    addresses->Array.map(({name, value}) => (name, value));
  };

  let getAliasMap = (~config) => {
    let%FResMap addresses = get(~config);

    addresses
    ->Array.map(((alias, addr)) => ((addr :> string), alias))
    ->Map.String.fromArray;
  };

  let getAliasForAddress = (~config, ~address: PublicKeyHash.t) => {
    let%FResMap aliases = getAliasMap(~config);
    aliases->Map.String.get((address :> string));
  };

  let getAddressForAlias = (~config, ~alias) => {
    let%FResMap addresses = get(~config);
    addresses->Map.String.fromArray->Map.String.get(alias);
  };

  let add = (~config, ~alias, ~address) =>
    Wallet.addOrReplacePkhAlias(
      ~dirpath=config->ConfigUtils.baseDir,
      ~alias,
      ~pkh=address,
      (),
    )
    ->Future.mapError(e => e->ErrorHandler.Wallet);

  let delete = (~config, ~alias) =>
    Wallet.removePkhAlias(~dirpath=config->ConfigUtils.baseDir, ~alias, ())
    ->Future.mapError(e => e->ErrorHandler.Wallet);

  type renameParams = {
    old_name: string,
    new_name: string,
  };

  let rename = (~config, renaming) =>
    Wallet.renameAlias(
      ~dirpath=config->ConfigUtils.baseDir,
      ~oldName=renaming.old_name,
      ~newName=renaming.new_name,
      (),
    )
    ->Future.mapError(e => e->ErrorHandler.Wallet);
};

module Accounts = {
  type t = array(Secret.t);
  type name = string;

  let isLedger = (pkh, secrets) => {
    secrets->Array.some((s: Secret.Repr.derived) =>
      s.secret.kind == Ledger && s.secret.addresses->Array.some((==)(pkh))
    );
  };

  let secrets = (~config as _) => {
    LocalStorage.getItem("secrets")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(Secret.decoder)))
    ->ResultEx.fromOption(ErrorHandler.(WalletAPI(NoSecretFound)));
  };

  let recoveryPhrases = (~config: ConfigFile.t) => {
    let _ = config;
    LocalStorage.getItem("recovery-phrases")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(SecureStorage.Cipher.decoder)));
  };

  let get = (~config) => {
    let%FRes pkhs =
      config
      ->ConfigUtils.baseDir
      ->Wallet.PkhAliases.read
      ->Future.mapError(e => e->ErrorHandler.Wallet);

    let%FResMap sks =
      config
      ->ConfigUtils.baseDir
      ->Wallet.SecretAliases.read
      ->Future.mapError(e => e->ErrorHandler.Wallet);

    pkhs->Array.keepMap(({name, value}) =>
      switch (sks->Wallet.SecretAliases.find(skAlias => name == skAlias.name)) {
      | Ok(_) => Some((name, value))
      | Error(_) => None
      }
    );
  };

  let secretAt = (~config, index) => {
    let%Res secrets = secrets(~config);

    ResultEx.fromOption(
      secrets[index],
      ErrorHandler.(WalletAPI(SecretNotFound(index))),
    );
  };

  let updateSecretAt = (~config, secret, index) => {
    let%Res secrets = secrets(~config);

    if (secrets[index] = secret) {
      LocalStorage.setItem(
        "secrets",
        Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
      )
      ->Ok;
    } else {
      Error(ErrorHandler.(WalletAPI(CannotUpdateSecret(index))));
    };
  };

  let recoveryPhraseAt = (~config, index, ~password) => {
    let%FRes data =
      recoveryPhrases(~config)
      ->Option.flatMap(recoveryPhrases => recoveryPhrases[index])
      ->FutureEx.fromOption(
          ~error=ErrorHandler.(WalletAPI(RecoveryPhraseNotFound(index))),
        );

    SecureStorage.Cipher.decrypt2(password, data)
    ->Future.mapError(_ =>
        ErrorHandler.(
          WalletAPI(Generic(I18n.form_input_error#wrong_password))
        )
      );
  };

  let import = (~config, ~alias, ~secretKey, ~password) => {
    let skUri = "encrypted:" ++ secretKey;

    let%FRes signer =
      ReTaquitoSigner.MemorySigner.create(
        ~secretKey,
        ~passphrase=password,
        (),
      )
      ->Future.mapError(e => e->ErrorHandler.Taquito);
    let%FRes pk =
      signer
      ->ReTaquitoSigner.publicKey
      ->Future.mapError(e => e->ErrorHandler.Taquito);
    let pk = Wallet.mnemonicPkValue(pk);
    let%FRes pkh =
      signer
      ->ReTaquitoSigner.publicKeyHash
      ->Future.mapError(e => e->ErrorHandler.Taquito);

    let%FRes () =
      Wallet.addOrReplaceAlias(
        ~dirpath=config->ConfigUtils.baseDir,
        ~alias,
        ~pk,
        ~pkh,
        ~sk=skUri,
        (),
      )
      ->Future.tapError(e => e->Js.log)
      ->Future.mapError(e => e->ErrorHandler.Wallet);

    pkh->FutureEx.ok;
  };

  let derive = (~config, ~index, ~alias, ~password) => {
    let%FRes secret = secretAt(~config, index)->Future.value;

    let%FRes recoveryPhrase = recoveryPhraseAt(~config, index, ~password);

    let%FRes edesk =
      secret.derivationPath
      ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
      ->HD.edesk(recoveryPhrase->HD.seed, ~password)
      ->Future.mapError(e => ErrorHandler.WalletAPI(Generic(e)));

    let%FRes address = import(~config, ~secretKey=edesk, ~alias, ~password);

    let%FRes () =
      {...secret, addresses: Array.concat(secret.addresses, [|address|])}
      ->updateSecretAt(~config, index)
      ->Future.value;

    address->FutureEx.ok;
  };

  let unsafeDelete = (~config, name) =>
    Wallet.removeAlias(~dirpath=config->ConfigUtils.baseDir, ~alias=name, ())
    ->Future.mapError(e => e->ErrorHandler.Wallet);

  let delete = (~config, name) => {
    let%FRes address = Aliases.getAddressForAlias(~config, ~alias=name);
    let%FRes () = unsafeDelete(~config, name);

    let%FRes secrets = secrets(~config)->Future.value;
    let secrets =
      secrets->Array.map(secret =>
        address == secret.masterPublicKey
          ? {...secret, masterPublicKey: None} : secret
      );
    let secrets = Json.Encode.array(Secret.encoder, secrets)->Json.stringify;

    LocalStorage.setItem("secrets", secrets)->FutureEx.ok;
  };

  let deleteSecretAt = (~config, index) => {
    let%FRes secretsBefore = secrets(~config)->Future.value;
    let%FRes aliases = Aliases.getAliasMap(~config);

    let%FRes deletedAddresses =
      secretsBefore[index]
      ->Option.map(secret =>
          secret.addresses
          ->Array.keepMap(v => aliases->Map.String.get((v :> string)))
        )
      ->FutureEx.fromOption(
          ~error=ErrorHandler.(WalletAPI(SecretNotFound(index))),
        );

    let%FResMap () =
      deletedAddresses->Array.reduce(Future.value(Ok()), (acc, addr) =>
        acc->Future.flatMapOk(() => addr->unsafeDelete(~config))
      );

    let _ =
      secretsBefore->Js.Array2.spliceInPlace(
        ~pos=index,
        ~remove=1,
        ~add=[||],
      );
    LocalStorage.setItem(
      "secrets",
      Json.Encode.array(Secret.encoder, secretsBefore)->Json.stringify,
    );

    switch (recoveryPhrases(~config)) {
    | Some(recoveryPhrases) =>
      let _ =
        recoveryPhrases->Js.Array2.spliceInPlace(
          ~pos=index,
          ~remove=1,
          ~add=[||],
        );
      LocalStorage.setItem(
        "recovery-phrases",
        Json.Encode.array(SecureStorage.Cipher.encoder, recoveryPhrases)
        ->Json.stringify,
      );
    | None => ()
    };

    let secretsAfter = secrets(~config);
    switch (secretsAfter) {
    | Ok([||])
    | Error(ErrorHandler.(WalletAPI(NoSecretFound))) =>
      "lock"->LocalStorage.removeItem;
      "recovery-phrases"->LocalStorage.removeItem;
      "secrets"->LocalStorage.removeItem;
    | _ => ()
    };
  };

  let used = (network, address) => {
    let%FResMap operations =
      network
      ->ServerAPI.Explorer.getOperations(address, ~limit=1, ())
      ->Future.mapError(s => ErrorHandler.(WalletAPI(Generic(s))));

    operations->Js.Array2.length != 0;
  };

  let legacyImport = (~config, alias, recoveryPhrase, ~password) => {
    let%FRes secretKey =
      HD.edeskLegacy(recoveryPhrase, ~password)
      ->Future.mapError(e => e->ErrorHandler.Generic->ErrorHandler.WalletAPI);

    import(~config, ~alias, ~secretKey, ~password);
  };

  module Scan = {
    type error =
      | APIError(string)
      | TaquitoError(ReTaquitoError.t);

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

        let%FRes address = getKey(path, schema);
        let found = () => {
          onFoundKey(n, address);
          loop(n + 1);
        };
        let%FRes used = used(config, address);
        used ? found() : n == 0 ? found() : Future.value(Ok());
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
        ~onFoundKey,
        path,
        schema,
        (path, schema) => {
          let%FRes tr = LedgerAPI.init();
          LedgerAPI.getKey(~prompt=false, tr, path, schema);
        },
      );

    let getSeedKey = (~recoveryPhrase, ~password, path, _) => {
      let%FRes secretKey =
        path
        ->HD.edesk(recoveryPhrase->HD.seed, ~password)
        ->Future.mapError(e => ErrorHandler.WalletAPI(Generic(e)));

      let%FRes signer =
        ReTaquitoSigner.MemorySigner.create(
          ~secretKey,
          ~passphrase=password,
          (),
        )
        ->Future.mapError(e => e->ErrorHandler.Taquito);

      signer
      ->ReTaquitoSigner.publicKeyHash
      ->Future.mapError(e => e->ErrorHandler.Taquito);
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
      switch (
        recoveryPhrases(~config)
        ->Option.flatMap(r => r[secret.Secret.Repr.index])
      ) {
      | Some(recoveryPhrase) =>
        let%FRes recoveryPhrase =
          recoveryPhrase
          ->SecureStorage.Cipher.decrypt(password)
          ->Future.mapError(e => ErrorHandler.(WalletAPI(Generic(e))));
        runStream(
          ~config,
          ~startIndex,
          ~onFoundKey,
          path,
          Wallet.Ledger.ED25519,
          getSeedKey(~recoveryPhrase, ~password),
        );
      | None => Future.value(Ok())
      };
    };

    let rec runOnSeed =
            (
              ~config: ConfigFile.t,
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
      let%FRes edesk =
        derivationPath
        ->DerivationPath.Pattern.implement(index)
        ->HD.edesk(seed, ~password)
        ->Future.mapError(e => ErrorHandler.WalletAPI(Generic(e)));

      let%FRes address =
        import(~config, ~secretKey=edesk, ~alias=name, ~password);

      let%FRes isValidated =
        // always include 0'
        index == 0 ? Future.value(Ok(true)) : config->used(address);

      if (isValidated) {
        let%FResMap addresses =
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
        unsafeDelete(~config, name)->Future.map(_ => Ok([||]));
      };
    };

    let runLegacy = (~config, recoveryPhrase, name, ~password) => {
      let%FRes legacyAddress =
        legacyImport(~config, name, recoveryPhrase, ~password);

      let%FRes isValidated = config->used(legacyAddress);

      isValidated
        ? Some(legacyAddress)->FutureEx.ok
        : unsafeDelete(~config, name)->Future.map(_ => Ok(None));
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
      let%FRes addresses =
        runOnSeed(
          ~config,
          recoveryPhrase->HD.seed,
          baseName,
          ~derivationPath,
          ~password,
          ~index,
          (),
        );
      let%FResMap legacyAddresses =
        runLegacy(~config, recoveryPhrase, baseName ++ " legacy", ~password);
      (addresses, legacyAddresses);
    };
  };

  let indexOfRecoveryPhrase = (~config, recoveryPhrase, ~password) =>
    recoveryPhrases(~config)
    ->Option.getWithDefault([||])
    ->Array.map(data =>
        SecureStorage.Cipher.decrypt2(password, data)
        ->Future.mapError(e => ErrorHandler.(WalletAPI(Generic(e))))
      )
    ->List.fromArray
    ->Future.all
    ->Future.map(List.toArray)
    ->Future.map(decryptedRecoveryPhrases =>
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
    LocalStorage.setItem(
      "secrets",
      Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
    );
  };

  let registerRecoveryPhrase = (~config, recoveryPhrase) =>
    recoveryPhrases(~config)
    ->Option.getWithDefault([||])
    ->(
        recoveryPhrases => {
          let recoveryPhrases =
            Array.concat(recoveryPhrases, [|recoveryPhrase|]);
          Json.Encode.(array(SecureStorage.Cipher.encoder, recoveryPhrases))
          |> Json.stringify
          |> LocalStorage.setItem("recovery-phrases");
        }
      );

  let restore =
      (
        ~config,
        ~backupPhrase,
        ~name,
        ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                          DerivationPath.Pattern.default,
                        ),
        ~derivationScheme=Wallet.Ledger.ED25519,
        ~password,
        (),
      ) => {
    let backupPhraseConcat = backupPhrase->Js.Array2.joinWith(" ");

    let%FRes () =
      password
      ->SecureStorage.validatePassword
      ->Future.mapError(_ =>
          ErrorHandler.(
            WalletAPI(Generic(I18n.form_input_error#wrong_password))
          )
        );

    let bp = backupPhrase->Array.length;

    let%FRes () =
      (
        if (bp == 24 || bp == 15 || bp == 12) {
          backupPhrase->Js.Array2.reducei(
            (res, w, i) =>
              res->Result.flatMap(() =>
                w->Bip39.included
                  ? Ok()
                  : ErrorHandler.(UnknownBip39Word(w, i)->WalletAPI->Error)
              ),
            Ok(),
          );
        } else {
          ErrorHandler.(IncorrectNumberOfWords->WalletAPI->Error);
        }
      )
      ->Future.value;

    let%FRes () =
      indexOfRecoveryPhrase(~config, backupPhraseConcat, ~password)
      ->Future.map(index =>
          switch (index) {
          | Some(_) => ErrorHandler.(WalletAPI(SecretAlreadyImported))->Error
          | None => Ok()
          }
        );

    let%FRes (addresses, legacyAddress) =
      Scan.run(
        ~config,
        ~recoveryPhrase=backupPhraseConcat,
        ~baseName=name,
        ~derivationPath,
        ~password,
        (),
      );

    let%FResMap () =
      backupPhraseConcat
      ->SecureStorage.Cipher.encrypt(password)
      ->Future.mapError(e => ErrorHandler.(WalletAPI(Generic(e))))
      ->Future.mapOk(registerRecoveryPhrase(~config));

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

  let importRemainingMnemonicKeys = (~config, ~password, ~index, ()) => {
    let%FRes secret = secretAt(~config, index)->Future.value;
    let%FRes recoveryPhrase = recoveryPhraseAt(~config, index, ~password);

    let%FlatRes (addresses, masterPublicKey) =
      Scan.run(
        ~config,
        ~recoveryPhrase,
        ~baseName=secret.name,
        ~derivationPath=secret.derivationPath,
        ~password,
        ~index=secret.addresses->Array.length,
        (),
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
        ~config,
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
      ~dirpath=config->ConfigUtils.baseDir,
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
        (),
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
          ->Future.flatMapOk(key =>
              importKeys(tr, [key, ...keys], index + 1)
            )
        : List.reverse(keys)->List.toArray->Ok->Future.value;
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
        ~derivationScheme=Wallet.Ledger.ED25519,
        ~ledgerMasterKey,
        (),
      ) => {
    let%FRes tr = LedgerAPI.init(~timeout?, ());
    let%FRes addresses =
      importLedgerKeys(
        ~config,
        ~accountsNumber,
        ~startIndex=0,
        ~basename=name,
        ~derivationPath,
        ~derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
        (),
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

    let%FResMap () =
      name
      ->SecureStorage.Cipher.encrypt("")
      ->Future.mapError(e => ErrorHandler.(WalletAPI(Generic(e))))
      ->Future.mapOk(registerRecoveryPhrase(~config));

    addresses;
  };

  let deriveLedger =
      (~config, ~timeout=?, ~index, ~alias, ~ledgerMasterKey, ()) => {
    let%FRes secret = secretAt(~config, index)->Future.value;
    let%FRes tr = LedgerAPI.init(~timeout?, ());

    let%FRes address =
      importLedgerKey(
        ~config,
        ~name=alias,
        ~index=secret.addresses->Array.length,
        ~derivationPath=secret.derivationPath,
        ~derivationScheme=secret.derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
      );

    let%FRes () =
      {...secret, addresses: Array.concat(secret.addresses, [|address|])}
      ->updateSecretAt(~config, index)
      ->Future.value;

    address->FutureEx.ok;
  };

  let deriveLedgerKeys =
      (~config, ~timeout=?, ~index, ~accountsNumber, ~ledgerMasterKey, ()) => {
    let%FRes secret = secretAt(~config, index)->Future.value;
    let%FRes tr = LedgerAPI.init(~timeout?, ());

    let%FRes addresses =
      importLedgerKeys(
        ~config,
        ~basename=secret.Secret.Repr.name,
        ~startIndex=secret.addresses->Array.length,
        ~accountsNumber,
        ~derivationPath=secret.derivationPath,
        ~derivationScheme=secret.derivationScheme,
        ~ledgerTransport=tr,
        ~ledgerMasterKey,
        (),
      );

    let%FRes () =
      {...secret, addresses: Array.concat(secret.addresses, addresses)}
      ->updateSecretAt(~config, index)
      ->Future.value;

    addresses->FutureEx.ok;
  };

  let getPublicKey = (~config: ConfigFile.t, ~account: Account.t) => {
    Wallet.pkFromAlias(
      ~dirpath=config->ConfigUtils.baseDir,
      ~alias=account.name,
      (),
    );
  };
};
