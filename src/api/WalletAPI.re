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

    let legacyAddressDecoder = json =>
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
      legacyAddress: json |> legacyAddressDecoder,
    };

  let kindToString =
    fun
    | Secret.Mnemonics => "mnemonics"
    | Ledger => "ledger";

  let encoder = secret =>
    Json.Encode.(
      switch (secret.Repr.legacyAddress) {
      | Some(legacyAddress) =>
        object_([
          ("name", string(secret.name)),
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
          ("legacyAddress", (legacyAddress :> string)->string),
        ])
      | None =>
        object_([
          ("name", string(secret.name)),
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
        ])
      }
    );
};

module Aliases = {
  type t = array((string, PublicKeyHash.t));

  let get = (~config) =>
    config
    ->ConfigUtils.baseDir
    ->Wallet.PkhAliases.read
    ->Future.mapError(e => e->ErrorHandler.Wallet)
    ->Future.mapOk(l => l->Array.map(({name, value}) => (name, value)));

  let getAliasMap = (~config) =>
    get(~config)
    ->Future.mapOk(addresses =>
        addresses->Array.map(((alias, addr)) => ((addr :> string), alias))
      )
    ->Future.mapOk(Map.String.fromArray);

  let getAliasForAddress = (~config, ~address: PublicKeyHash.t) =>
    getAliasMap(~config)
    ->Future.mapOk(aliases => aliases->Map.String.get((address :> string)));

  let getAddressForAlias = (~config, ~alias) =>
    get(~config)
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Map.String.get(alias));

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

  let secrets = (~config as _: ConfigFile.t) => {
    LocalStorage.getItem("secrets")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(Secret.decoder)))
    ->ResultEx.fromOption(Error(ErrorHandler.(WalletAPI(NoSecretFound))));
  };

  let recoveryPhrases = (~config: ConfigFile.t) => {
    let _ = config;
    LocalStorage.getItem("recovery-phrases")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(SecureStorage.Cipher.decoder)));
  };

  let get = (~config) =>
    config
    ->ConfigUtils.baseDir
    ->Wallet.PkhAliases.read
    ->Future.flatMapOk(pkhs => {
        config
        ->ConfigUtils.baseDir
        ->Wallet.SecretAliases.read
        ->Future.mapOk(sks =>
            pkhs->Array.keepMap(({name, value}) =>
              switch (
                sks->Wallet.SecretAliases.find(skAlias => name == skAlias.name)
              ) {
              | Ok(_) => Some((name, value))
              | Error(_) => None
              }
            )
          )
      })
    ->Future.mapError(e => e->ErrorHandler.Wallet);

  let secretAt = (~config, index) =>
    secrets(~config)
    ->Future.value
    ->Future.flatMapOk(secrets =>
        secrets[index]
        ->FutureEx.fromOption(
            ~error=ErrorHandler.(WalletAPI(SecretNotFound(index))),
          )
      );

  let updateSecretAt = (~config, secret, index) =>
    secrets(~config)
    ->Future.value
    ->Future.flatMapOk(secrets => {
        (secrets[index] = secret)
          ? Future.value(
              Ok(
                LocalStorage.setItem(
                  "secrets",
                  Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
                ),
              ),
            )
          : Future.value(
              Error(ErrorHandler.(WalletAPI(CannotUpdateSecret(index)))),
            )
      });

  let recoveryPhraseAt = (~config, index, ~password) =>
    recoveryPhrases(~config)
    ->Option.flatMap(recoveryPhrases => recoveryPhrases[index])
    ->FutureEx.fromOption(
        ~error=ErrorHandler.(WalletAPI(RecoveryPhraseNotFound(index))),
      )
    ->Future.flatMapOk(data =>
        SecureStorage.Cipher.decrypt2(password, data)
        ->Future.mapError(_ =>
            ErrorHandler.(
              WalletAPI(Generic(I18n.form_input_error#wrong_password))
            )
          )
      );

  let import = (~config, ~alias, ~secretKey, ~password) => {
    let skUri = "encrypted:" ++ secretKey;
    let keys =
      ReTaquitoSigner.MemorySigner.create(
        ~secretKey,
        ~passphrase=password,
        (),
      )
      ->Future.flatMapOk(s =>
          s
          ->ReTaquitoSigner.publicKey
          ->Future.flatMapOk(pk =>
              s
              ->ReTaquitoSigner.publicKeyHash
              ->Future.mapOk(pkh => (Wallet.mnemonicPkValue(pk), pkh, skUri))
            )
        )
      ->Future.mapError(e => e->ErrorHandler.Taquito);
    keys->Future.flatMapOk(((pk, pkh, sk)) =>
      Wallet.addOrReplaceAlias(
        ~dirpath=config->ConfigUtils.baseDir,
        ~alias,
        ~pk,
        ~pkh,
        ~sk,
        (),
      )
      ->Future.tapError(e => e->Js.log)
      ->Future.mapError(e => e->ErrorHandler.Wallet)
      ->Future.mapOk(() => pkh)
    );
  };

  let derive = (~config, ~index, ~alias, ~password) =>
    Future.mapOk2(
      secretAt(~config, index),
      recoveryPhraseAt(~config, index, ~password),
      (secret, recoveryPhrase) => {
      secret.derivationPath
      ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
      ->HD.edesk(recoveryPhrase->HD.seed, ~password)
      ->Future.mapError(e => ErrorHandler.WalletAPI(Generic(e)))
      ->Future.flatMapOk(edesk =>
          import(~config, ~secretKey=edesk, ~alias, ~password)
        )
      ->Future.tapOk(address =>
          {
            ...secret,
            addresses: Array.concat(secret.addresses, [|address|]),
          }
          ->updateSecretAt(~config, index)
        )
    })
    ->Future.flatMapOk(update => update);

  let unsafeDelete = (~config, name) =>
    Wallet.removeAlias(~dirpath=config->ConfigUtils.baseDir, ~alias=name, ())
    ->Future.mapError(e => e->ErrorHandler.Wallet);

  let delete = (~config, name) =>
    Aliases.getAddressForAlias(~config, ~alias=name)
    ->Future.flatMapOk(address =>
        unsafeDelete(~config, name)
        ->Future.flatMapOk(_ =>
            secrets(~config)
            ->Result.map(secrets =>
                secrets->Array.map(secret =>
                  address == secret.legacyAddress
                    ? {...secret, legacyAddress: None} : secret
                )
              )
            ->Result.map(secrets =>
                Json.Encode.array(Secret.encoder, secrets)->Json.stringify
              )
            ->Result.map("secrets"->LocalStorage.setItem)
            ->Future.value
          )
      );

  let deleteSecretAt = (~config, index) =>
    Future.mapOk2(
      secrets(~config)->Future.value,
      Aliases.getAliasMap(~config),
      (secrets, aliases) => {
      secrets[index]
      ->Option.map(secret =>
          secret.addresses
          ->Array.keepMap(v => aliases->Map.String.get((v :> string)))
        )
      ->FutureEx.fromOption(
          ~error=ErrorHandler.(WalletAPI(SecretNotFound(index))),
        )
      ->Future.flatMapOk(array =>
          array->Array.reduce(Future.value(Ok([||])), (a, b) =>
            a->Future.flatMapOk(a =>
              b
              ->unsafeDelete(~config)
              ->Future.mapOk(b => a->Array.concat([|b|]))
            )
          )
        )
      ->Future.tapOk(_ => {
          let _ =
            secrets->Js.Array2.spliceInPlace(
              ~pos=index,
              ~remove=1,
              ~add=[||],
            );
          LocalStorage.setItem(
            "secrets",
            Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
          );
        })
      ->Future.tapOk(_ =>
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
          }
        )
    })
    ->Future.flatMapOk(update => update)
    ->Future.tapOk(_ => {
        let recoveryPhrases = recoveryPhrases(~config);
        if (recoveryPhrases == Some([||]) || recoveryPhrases == None) {
          "lock"->LocalStorage.removeItem;
          "recovery-phrases"->LocalStorage.removeItem;
          "secrets"->LocalStorage.removeItem;
        };
      });

  let used = (network, address) => {
    network
    ->ServerAPI.Explorer.getOperations(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0})
    ->Future.mapError(s => ErrorHandler.(WalletAPI(Generic(s))));
  };

  let legacyImport = (~config, alias, recoveryPhrase, ~password) =>
    HD.edeskLegacy(recoveryPhrase, ~password)
    ->Future.mapError(e => e->ErrorHandler.Generic->ErrorHandler.WalletAPI)
    ->Future.flatMapOk(secretKey =>
        import(~config, ~alias, ~secretKey, ~password)
      );

  module Scan = {
    type error =
      | APIError(string)
      | TaquitoError(ReTaquitoError.t);

    let used = (network, address) => {
      network
      ->ServerAPI.Explorer.getOperations(address, ~limit=1, ())
      ->Future.mapOk(operations => {operations->Js.Array2.length != 0})
      ->Future.mapError(s => ErrorHandler.(WalletAPI(Generic(s))));
    };

    let runStream =
        (config, onFoundKey, path: DerivationPath.Pattern.t, schema) => {
      let rec loop = n => {
        let path = path->DerivationPath.Pattern.implement(n);
        LedgerAPI.init()
        ->Future.flatMapOk(tr =>
            LedgerAPI.getKey(~prompt=false, tr, path, schema)
          )
        ->Future.flatMapOk(address => {
            let found = () => {
              Js.log(address);
              onFoundKey(n, address);
              loop(n + 1);
            };

            used(config, address)
            ->Future.flatMapOk(
                fun
                | true => found()
                | false when n == 0 => found()
                | false => Future.value(Ok()),
              );
          });
      };
      loop(0);
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
      derivationPath
      ->DerivationPath.Pattern.implement(index)
      ->HD.edesk(seed, ~password)
      ->Future.mapError(e => ErrorHandler.WalletAPI(Generic(e)))
      ->Future.flatMapOk(edesk =>
          import(~config, ~secretKey=edesk, ~alias=name, ~password)
          ->Future.flatMapOk(address
              // always include 0'
              =>
                (
                  index == 0 ? Future.value(Ok(true)) : config->used(address)
                )
                ->Future.flatMapOk(isValidated =>
                    if (isValidated) {
                      runOnSeed(
                        ~config,
                        seed,
                        baseName,
                        ~derivationPath,
                        ~password,
                        ~index=index + 1,
                        (),
                      )
                      ->Future.mapOk(addresses =>
                          Array.concat([|address|], addresses)
                        );
                    } else {
                      unsafeDelete(~config, name)->Future.map(_ => Ok([||]));
                    }
                  )
              )
        );
    };

    let runLegacy = (~config, recoveryPhrase, name, ~password) =>
      legacyImport(~config, name, recoveryPhrase, ~password)
      ->Future.flatMapOk(legacyAddress =>
          config
          ->used(legacyAddress)
          ->Future.mapOk(isValidated =>
              isValidated ? Some(legacyAddress) : None
            )
        )
      ->Future.flatMapOk(legacyAddress =>
          legacyAddress == None
            ? unsafeDelete(~config, name)->Future.map(_ => Ok(None))
            : Future.value(Ok(legacyAddress))
        );

    let run =
        (
          ~config,
          recoveryPhrase,
          baseName,
          ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                            DerivationPath.Pattern.default,
                          ),
          ~password,
          ~index=0,
          (),
        ) =>
      runOnSeed(
        ~config,
        recoveryPhrase->HD.seed,
        baseName,
        ~derivationPath,
        ~password,
        ~index,
        (),
      )
      ->Future.flatMapOk(addresses => {
          runLegacy(~config, recoveryPhrase, baseName ++ " legacy", ~password)
          ->Future.mapOk(legacyAddresses => (addresses, legacyAddresses))
        });

    let runAll = (~config, ~password) =>
      (
        switch (recoveryPhrases(~config), secrets(~config)) {
        | (Some(recoveryPhrases), Ok(secrets)) =>
          Array.zip(recoveryPhrases, secrets)
          ->Array.map(((recoveryPhrase, secret)) =>
              recoveryPhrase
              ->SecureStorage.Cipher.decrypt(password)
              ->Future.mapError(e => ErrorHandler.(WalletAPI(Generic(e))))
              ->Future.flatMapOk(recoveryPhrase =>
                  run(
                    ~config,
                    recoveryPhrase,
                    secret.name,
                    ~derivationPath=secret.derivationPath,
                    ~password,
                    ~index=secret.addresses->Array.length,
                    (),
                  )
                )
              ->Future.mapOk(((addresses, legacyAddress)) =>
                  {
                    ...secret,
                    addresses: secret.addresses->Array.concat(addresses),
                    legacyAddress,
                  }
                )
            )
          ->List.fromArray
        | _ => []
        }
      )
      ->Future.all
      ->Future.map(results => {
          let error = results->List.getBy(Result.isError);
          switch (error) {
          | Some(Error(error)) => Error(error)
          | _ => Ok(results->List.toArray)
          };
        })
      ->Future.mapOk(secrets =>
          secrets->Array.keepMap(secret =>
            switch (secret) {
            | Ok(secret) => Some(secret)
            | _ => None
            }
          )
        )
      ->Future.mapOk(secrets =>
          Json.Encode.array(Secret.encoder, secrets)->Json.stringify
        )
      ->Future.mapOk(LocalStorage.setItem("secrets"));
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
        ~legacyAddress,
      ) => {
    let secret = {
      Secret.Repr.name,
      kind,
      derivationPath,
      derivationScheme,
      addresses,
      legacyAddress,
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
    password
    ->SecureStorage.validatePassword
    ->Future.mapError(_ =>
        ErrorHandler.(
          WalletAPI(Generic(I18n.form_input_error#wrong_password))
        )
      )
    ->Future.flatMapOk(_ =>
        (
          switch (backupPhrase->Array.length) {
          | 24
          | 15
          | 12 =>
            backupPhrase->Js.Array2.reducei(
              (res, w, i) =>
                res->Result.flatMap(() =>
                  w->Bip39.included
                    ? Ok()
                    : ErrorHandler.(UnknownBip39Word(w, i)->WalletAPI->Error)
                ),
              Ok(),
            )
          | _ => ErrorHandler.(IncorrectNumberOfWords->WalletAPI->Error)
          }
        )
        ->Future.value
      )
    ->Future.flatMapOk(_ =>
        indexOfRecoveryPhrase(~config, backupPhraseConcat, ~password)
        ->Future.map(index =>
            switch (index) {
            | Some(_) =>
              Error(ErrorHandler.(WalletAPI(SecretAlreadyImported)))
            | None => Ok(index)
            }
          )
      )
    ->Future.flatMapOk(_ =>
        Scan.run(
          ~config,
          backupPhraseConcat,
          name,
          ~derivationPath,
          ~password,
          (),
        )
      )
    ->Future.tapOk(_ =>
        Future.mapOk2(
          recoveryPhrases(~config)
          ->FutureEx.fromOptionWithDefault(~default=[||]),
          backupPhraseConcat->SecureStorage.Cipher.encrypt(password),
          (recoveryPhrases, newRecoveryPhrase) => {
          Array.concat(recoveryPhrases, [|newRecoveryPhrase|])
        })
        ->Future.mapOk(Json.Encode.(array(SecureStorage.Cipher.encoder)))
        ->Future.mapOk(json => json->Json.stringify)
        ->FutureEx.getOk(recoveryPhrases =>
            LocalStorage.setItem("recovery-phrases", recoveryPhrases)
          )
      )
    ->Future.tapOk(((addresses, legacyAddress)) =>
        registerSecret(
          ~config,
          ~name,
          ~kind=Secret.Repr.Mnemonics,
          ~derivationPath,
          ~derivationScheme,
          ~addresses,
          ~legacyAddress,
        )
      );
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

  let importLedger =
      (
        ~config,
        ~name,
        ~accountsNumber,
        ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                          DerivationPath.Pattern.default,
                        ),
        ~derivationScheme=Wallet.Ledger.ED25519,
        ~ledgerMasterKey,
        (),
      ) => {
    let rec importKeys = (tr, keys, index) => {
      let name = name ++ " /" ++ Int.toString(index);
      index < accountsNumber
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
    LedgerAPI.init()
    ->Future.flatMapOk(tr => tr->importKeys([], 0))
    ->Future.tapOk(addresses =>
        registerSecret(
          ~config,
          ~name,
          ~kind=Secret.Repr.Ledger,
          ~derivationPath,
          ~derivationScheme,
          ~addresses,
          ~legacyAddress=None,
        )
      );
  };

  let getPublicKey = (~config: ConfigFile.t, ~account: Account.t) => {
    Wallet.pkFromAlias(
      ~dirpath=config->ConfigUtils.baseDir,
      ~alias=account.name,
      (),
    );
  };
};
