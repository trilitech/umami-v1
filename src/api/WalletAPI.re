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
  type t = {
    name: string,
    derivationPath: DerivationPath.Pattern.t,
    addresses: Js.Array.t(PublicKeyHash.t),
    legacyAddress: option(PublicKeyHash.t),
  };

  let decoder = json =>
    Json.Decode.{
      name: json |> field("name", string),
      derivationPath:
        json
        |> field("derivationScheme", string)
        |> DerivationPath.Pattern.fromString
        |> Result.getExn,
      addresses:
        (json |> field("addresses", array(string)))
        ->Array.map(v => v->PublicKeyHash.build->Result.getExn),

      legacyAddress:
        (json |> optional(field("legacyAddress", string)))
        ->Option.map(v => v->PublicKeyHash.build->Result.getExn),
    };

  let encoder = secret =>
    Json.Encode.(
      switch (secret.legacyAddress) {
      | Some(legacyAddress) =>
        object_([
          ("name", string(secret.name)),
          (
            "derivationScheme",
            string(secret.derivationPath->DerivationPath.Pattern.toString),
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
          (
            "derivationScheme",
            string(secret.derivationPath->DerivationPath.Pattern.toString),
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

  let get = (~settings) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapError(ErrorHandler.fromSdkToString)
    ->Future.mapOk(l => l->Array.map(({alias, pkh}) => (alias, pkh)));

  let getAliasMap = (~settings) =>
    get(~settings)
    ->Future.mapOk(addresses =>
        addresses->Array.map(((alias, addr)) => ((addr :> string), alias))
      )
    ->Future.mapOk(Map.String.fromArray);

  let getAliasForAddress = (~settings, ~address: PublicKeyHash.t) =>
    getAliasMap(~settings)
    ->Future.mapOk(aliases => aliases->Map.String.get((address :> string)));

  let getAddressForAlias = (~settings, ~alias) =>
    get(~settings)
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Map.String.get(alias));

  let add = (~settings, ~alias, ~address) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.addAddress(alias, address)
    ->Future.mapError(ErrorHandler.fromSdkToString);

  let delete = (~settings, ~alias) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.forgetAddress(alias)
    ->Future.mapError(ErrorHandler.fromSdkToString);

  let rename = (~settings, renaming) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.renameAliases(renaming)
    ->Future.mapError(ErrorHandler.fromSdkToString);
};

module Accounts = {
  type t = array(Secret.t);
  type name = string;

  let secrets = (~settings: AppSettings.t) => {
    let _ = settings;
    LocalStorage.getItem("secrets")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(Secret.decoder)));
  };

  let recoveryPhrases = (~settings: AppSettings.t) => {
    let _ = settings;
    LocalStorage.getItem("recovery-phrases")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(SecureStorage.Cipher.decoder)));
  };

  let get = (~settings: AppSettings.t) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapError(ErrorHandler.fromSdkToString)
    ->Future.mapOk(r =>
        r->Array.keepMap((TezosSDK.OutputAddress.{alias, pkh, sk_known}) =>
          sk_known ? Some((alias, pkh)) : None
        )
      );

  let secretAt = (~settings, index) =>
    secrets(~settings)
    ->FutureEx.fromOption(~error="No secrets found!")
    ->Future.flatMapOk(secrets =>
        secrets[index]
        ->FutureEx.fromOption(
            ~error="Secret at index " ++ index->Int.toString ++ " not found!",
          )
      );

  let updateSecretAt = (~settings, secret, index) =>
    secrets(~settings)
    ->FutureEx.fromOption(~error="No secrets found!")
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
              Error(
                "Can't update secret at index " ++ index->Int.toString ++ "!",
              ),
            )
      });

  let recoveryPhraseAt = (~settings, index, ~password) =>
    recoveryPhrases(~settings)
    ->Option.flatMap(recoveryPhrases => recoveryPhrases[index])
    ->FutureEx.fromOption(
        ~error=
          "Recovery phrase at index " ++ index->Int.toString ++ " not found!",
      )
    ->Future.flatMapOk(SecureStorage.Cipher.decrypt2(password))
    ->Future.mapError(_ => {I18n.form_input_error#wrong_password});

  let add = (~settings, ~alias, ~address) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, address);

  let import = (~settings, ~alias, ~secretKey, ~password) => {
    let skUri = "encrypted:" ++ secretKey;
    settings
    ->AppSettings.sdk
    ->TezosSDK.importSecretKey(~name=alias, ~skUri, ~password, ())
    ->Future.mapError(ErrorHandler.fromSdkToString)
    ->Future.tapOk(k => Js.log("key found : " ++ (k :> string)));
  };

  let derive = (~settings, ~index, ~alias, ~password) =>
    Future.mapOk2(
      secretAt(~settings, index),
      recoveryPhraseAt(~settings, index, ~password),
      (secret, recoveryPhrase) => {
      secret.derivationPath
      ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
      ->HD.edesk(recoveryPhrase->HD.seed, ~password)
      ->Future.flatMapOk(edesk =>
          import(~settings, ~secretKey=edesk, ~alias, ~password)
        )
      ->Future.tapOk(address =>
          {
            ...secret,
            addresses: Array.concat(secret.addresses, [|address|]),
          }
          ->updateSecretAt(~settings, index)
        )
    })
    ->Future.flatMapOk(update => update);

  let unsafeDelete = (~settings, name) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.forgetAddress(name)
    ->Future.mapError(ErrorHandler.fromSdkToString);

  let delete = (~settings, name) =>
    Aliases.getAddressForAlias(~settings, ~alias=name)
    ->Future.flatMapOk(address =>
        unsafeDelete(~settings, name)
        ->Future.mapOk(_ =>
            secrets(~settings)
            ->Option.map(secrets =>
                secrets->Array.map(secret =>
                  address == secret.legacyAddress
                    ? {...secret, legacyAddress: None} : secret
                )
              )
            ->Option.map(secrets =>
                Json.Encode.array(Secret.encoder, secrets)->Json.stringify
              )
            ->Option.map("secrets"->LocalStorage.setItem)
          )
      );

  let deleteSecretAt = (~settings, index) =>
    Future.mapOk2(
      secrets(~settings)->FutureEx.fromOption(~error="No secrets found!"),
      Aliases.getAliasMap(~settings),
      (secrets, aliases) => {
      secrets[index]
      ->Option.map(secret =>
          secret.addresses
          ->Array.keepMap(v => aliases->Map.String.get((v :> string)))
        )
      ->FutureEx.fromOption(
          ~error="Secret at index " ++ index->Int.toString ++ " not found!",
        )
      ->Future.flatMapOk(array =>
          array->Array.reduce(Future.value(Ok([||])), (a, b) =>
            a->Future.flatMapOk(a =>
              b
              ->unsafeDelete(~settings)
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
          switch (recoveryPhrases(~settings)) {
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
        let recoveryPhrases = recoveryPhrases(~settings);
        if (recoveryPhrases == Some([||]) || recoveryPhrases == None) {
          "lock"->LocalStorage.removeItem;
          "recovery-phrases"->LocalStorage.removeItem;
          "secrets"->LocalStorage.removeItem;
        };
      });

  let used = (network, address) => {
    network
    ->ServerAPI.Explorer.getOperations(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0});
  };

  let rec scanSeed =
          (
            ~settings: AppSettings.t,
            seed,
            baseName,
            ~derivationPath=DerivationPath.Pattern.default->DerivationPath.Pattern.fromTezosBip44,
            ~password,
            ~index=0,
            (),
          ) => {
    let name = baseName ++ " /" ++ index->Js.Int.toString;
    derivationPath
    ->DerivationPath.Pattern.implement(index)
    ->HD.edesk(seed, ~password)
    ->Future.flatMapOk(edesk =>
        import(~settings, ~secretKey=edesk, ~alias=name, ~password)
        ->Future.flatMapOk(address
            // always include 0'
            =>
              (
                index == 0 ? Future.value(Ok(true)) : settings->used(address)
              )
              ->Future.flatMapOk(isValidated =>
                  if (isValidated) {
                    scanSeed(
                      ~settings,
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
                    unsafeDelete(~settings, name)->Future.map(_ => Ok([||]));
                  }
                )
            )
      );
  };

  let legacyImport = (~settings, name, recoveryPhrase, ~password) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.importKeysFromMnemonics(
        ~name,
        ~mnemonics=recoveryPhrase,
        ~password,
        (),
      )
    ->Future.mapError(ErrorHandler.fromSdkToString);

  let legacyScan = (~settings, recoveryPhrase, name, ~password) =>
    legacyImport(~settings, name, recoveryPhrase, ~password)
    ->Future.flatMapOk(legacyAddress =>
        settings
        ->used(legacyAddress)
        ->Future.mapOk(isValidated =>
            isValidated ? Some(legacyAddress) : None
          )
      )
    ->Future.flatMapOk(legacyAddress =>
        legacyAddress == None
          ? unsafeDelete(~settings, name)->Future.map(_ => Ok(None))
          : Future.value(Ok(legacyAddress))
      );

  let scan =
      (
        ~settings,
        recoveryPhrase,
        baseName,
        ~derivationPath=DerivationPath.Pattern.default->DerivationPath.Pattern.fromTezosBip44,
        ~password,
        ~index=0,
        (),
      ) =>
    scanSeed(
      ~settings,
      recoveryPhrase->HD.seed,
      baseName,
      ~derivationPath,
      ~password,
      ~index,
      (),
    )
    ->Future.flatMapOk(addresses => {
        legacyScan(
          ~settings,
          recoveryPhrase,
          baseName ++ " legacy",
          ~password,
        )
        ->Future.mapOk(legacyAddresses => (addresses, legacyAddresses))
      });

  let indexOfRecoveryPhrase = (~settings, recoveryPhrase, ~password) =>
    recoveryPhrases(~settings)
    ->Option.getWithDefault([||])
    ->Array.map(SecureStorage.Cipher.decrypt2(password))
    ->List.fromArray
    ->Future.all
    ->Future.map(List.toArray)
    ->Future.map(decryptedRecoveryPhrases =>
        decryptedRecoveryPhrases->Array.getBy(decryptedRecoveryPhrase =>
          decryptedRecoveryPhrase == Ok(recoveryPhrase)
        )
      );

  let restore =
      (
        ~settings,
        ~backupPhrase,
        ~name,
        ~derivationPath=DerivationPath.Pattern.fromTezosBip44(
                          DerivationPath.Pattern.default,
                        ),
        ~password,
        (),
      ) => {
    password
    ->SecureStorage.validatePassword
    ->Future.mapError(_ => I18n.form_input_error#wrong_password)
    ->Future.flatMapOk(_ =>
        indexOfRecoveryPhrase(~settings, backupPhrase, ~password)
        ->Future.map(index =>
            switch (index) {
            | Some(_) => Error("Secret already imported!")
            | None => Ok(index)
            }
          )
      )
    ->Future.flatMapOk(_ =>
        scan(~settings, backupPhrase, name, ~derivationPath, ~password, ())
      )
    ->Future.tapOk(_ =>
        Future.mapOk2(
          recoveryPhrases(~settings)
          ->FutureEx.fromOptionWithDefault(~default=[||]),
          backupPhrase->SecureStorage.Cipher.encrypt(password),
          (recoveryPhrases, newRecoveryPhrase) => {
          Array.concat(recoveryPhrases, [|newRecoveryPhrase|])
        })
        ->Future.mapOk(Json.Encode.(array(SecureStorage.Cipher.encoder)))
        ->Future.mapOk(json => json->Json.stringify)
        ->FutureEx.getOk(recoveryPhrases =>
            LocalStorage.setItem("recovery-phrases", recoveryPhrases)
          )
      )
    ->Future.tapOk(((addresses, legacyAddress)) => {
        let secret = {Secret.name, derivationPath, addresses, legacyAddress};
        let secrets =
          secrets(~settings)
          ->Option.getWithDefault([||])
          ->Array.concat([|secret|]);
        LocalStorage.setItem(
          "secrets",
          Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
        );
      });
  };

  let scanAll = (~settings, ~password) =>
    (
      switch (recoveryPhrases(~settings), secrets(~settings)) {
      | (Some(recoveryPhrases), Some(secrets)) =>
        Array.zip(recoveryPhrases, secrets)
        ->Array.map(((recoveryPhrase, secret)) =>
            recoveryPhrase
            ->SecureStorage.Cipher.decrypt(password)
            ->Future.flatMapOk(recoveryPhrase =>
                scan(
                  ~settings,
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

  let getPublicKey = (~settings: AppSettings.t, ~account: Account.t) => {
    Wallet.pkFromAlias(
      ~dirpath=settings->AppSettings.baseDir,
      ~alias=account.name,
      (),
    );
  };
};
