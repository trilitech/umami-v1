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

open ServerAPI;
open Delegate;

module Balance = {
  let get = (settings, address, ~params=?, ()) => {
    AppSettings.endpoint(settings)
    ->ReTaquito.Balance.get(~address, ~params?, ())
    ->Future.mapOk(Tez.ofInt64);
  };
};

module Error = {
  type token =
    | OperationNotRunnableOffchain(string)
    | SimulationNotAvailable(string)
    | InjectionNotImplemented(string)
    | OffchainCallNotImplemented(string)
    | RawError(string);

  type t =
    | Taquito(ReTaquitoError.t)
    | Token(token);

  let taquito = e => Taquito(e);
  let token = e => Token(e);

  let fromTaquitoToString = e =>
    e->ReTaquitoError.(
         fun
         | Generic(s) => s
         | WrongPassword => I18n.form_input_error#wrong_password
         | UnregisteredDelegate => I18n.form_input_error#unregistered_delegate
         | UnchangedDelegate => I18n.form_input_error#change_baker
         | BadPkh => I18n.form_input_error#bad_pkh
         | BranchRefused => I18n.form_input_error#branch_refused_error
         | InvalidContract => I18n.form_input_error#invalid_contract
         | EmptyTransaction => I18n.form_input_error#empty_transaction
         | WalletError(KeyNotFound) => I18n.wallet#key_not_found
         | WalletError(Generic(s)) => s
       );

  let printError = (fmt, err) => {
    switch (err) {
    | OperationNotRunnableOffchain(s) =>
      Format.fprintf(fmt, "Operation '%s' cannot be run offchain.", s)
    | SimulationNotAvailable(s) =>
      Format.fprintf(fmt, "Operation '%s' is not simulable.", s)
    | InjectionNotImplemented(s) =>
      Format.fprintf(fmt, "Operation '%s' injection is not implemented", s)
    | OffchainCallNotImplemented(s) =>
      Format.fprintf(
        fmt,
        "Operation '%s' offchain call is not implemented",
        s,
      )
    | RawError(s) => Format.fprintf(fmt, "%s", s)
    };
  };

  let fromTokenToString = err => Format.asprintf("%a", printError, err);

  let fromApiToString =
    fun
    | Token(e) => fromTokenToString(e)
    | Taquito(e) => fromTaquitoToString(e);

  let fromSdkToString = e =>
    e->TezosSDK.Error.(
         fun
         | Generic(s) => s
         | BadPkh => I18n.form_input_error#bad_pkh
       );
};

module Simulation = {
  let extractCustomValues = (tx_options: ProtocolOptions.transferOptions) => (
    tx_options.fee->Option.map(fee => fee->Tez.unsafeToMutezInt),
    tx_options.storageLimit,
    tx_options.gasLimit,
  );

  let batch = (settings, transfers, ~source, ~index=?, ()) => {
    let customValues =
      List.map(transfers, tx => tx.Transfer.tx_options->extractCustomValues)
      ->List.toArray;

    let transfers = (cache, source) =>
      transfers->ReTaquito.Transfer.prepareTransfers(cache, source);

    ReTaquito.Transfer.Estimate.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      (),
    )
    ->Future.flatMapOk(r =>
        ReTaquito.Estimate.handleEstimationResults(r, customValues, index)
      )
    ->Future.mapOk(
        ({customFeeMutez, burnFeeMutez, gasLimit, storageLimit, revealFee}) => {
        Protocol.{
          fee: (customFeeMutez + burnFeeMutez)->Tez.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->Tez.fromMutezInt,
        }
      });
  };

  let setDelegate = (settings, delegation: Protocol.delegation) => {
    ReTaquito.Estimate.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source=delegation.Protocol.source,
      ~delegate=?delegation.Protocol.delegate,
      ~fee=?delegation.Protocol.options.fee,
      (),
    )
    ->Future.mapOk(
        ({customFeeMutez, burnFeeMutez, gasLimit, storageLimit, revealFee}) =>
        Protocol.{
          fee: (customFeeMutez + burnFeeMutez)->Tez.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->Tez.fromMutezInt,
        }
      );
  };

  let run = (settings, ~index=?, operation: Protocol.t) => {
    switch (operation, index) {
    | (Delegation(d), _) => setDelegate(settings, d)
    | (Transaction({transfers, source}), None) =>
      batch(settings, transfers, ~source, ())
    | (Transaction({transfers, source}), Some(index)) =>
      batch(settings, transfers, ~source, ~index, ())
    };
  };
};

module Operation = {
  let batch = (settings, transfers, ~source, ~password) => {
    let transfers = (cache, source) =>
      transfers->ReTaquito.Transfer.prepareTransfers(cache, source);

    ReTaquito.Transfer.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      ~password,
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let setDelegate =
      (settings, Protocol.{delegate, source, options}, ~password) => {
    ReTaquito.Operations.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~delegate,
      ~password,
      ~fee=?options.fee->Option.map(Tez.toInt64),
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let run = (settings, operation: Protocol.t, ~password) =>
    switch (operation) {
    | Delegation(d) => setDelegate(settings, d, ~password)

    | Transaction({transfers, source}) =>
      batch(settings, transfers, ~source, ~password)
    };
};

module MapString = Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module Secret = {
  type t = {
    name: string,
    derivationPath: DerivationPath.Pattern.t,
    addresses: Js.Array.t(string),
    legacyAddress: option(string),
  };

  let decoder = json =>
    Json.Decode.{
      name: json |> field("name", string),
      derivationPath:
        json
        |> field("derivationScheme", string)
        |> DerivationPath.Pattern.fromString
        |> Result.getExn,

      addresses: json |> field("addresses", array(string)),
      legacyAddress: json |> optional(field("legacyAddress", string)),
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
          ("addresses", stringArray(secret.addresses)),
          ("legacyAddress", string(legacyAddress)),
        ])
      | None =>
        object_([
          ("name", string(secret.name)),
          (
            "derivationScheme",
            string(secret.derivationPath->DerivationPath.Pattern.toString),
          ),
          ("addresses", stringArray(secret.addresses)),
        ])
      }
    );
};

module Aliases = {
  let parse = content =>
    content
    |> Js.String.split("\n")
    |> Js.Array.map(row => row |> Js.String.split(": "))
    |> (pairs => pairs->Js.Array2.filter(pair => pair->Array.length == 2))
    |> Js.Array.map(pair =>
         (pair->Array.getUnsafe(0), pair->Array.getUnsafe(1))
       );

  let get = (~settings) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapError(Error.fromSdkToString)
    ->Future.mapOk(l => l->Array.map(({alias, pkh}) => (alias, pkh)));

  let getAliasMap = (~settings) =>
    get(~settings)
    ->Future.mapOk(addresses => addresses->Array.map(((a, b)) => (b, a)))
    ->Future.mapOk(Map.String.fromArray);

  let getAliasForAddress = (~settings, address) =>
    getAliasMap(~settings)
    ->Future.mapOk(aliases => aliases->Map.String.get(address));

  let getAddressForAlias = (~settings, alias) =>
    get(~settings)
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Map.String.get(alias));

  let add = (~settings, alias, pkh) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.addAddress(alias, pkh)
    ->Future.mapError(Error.fromSdkToString);

  let delete = (~settings, name) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.forgetAddress(name)
    ->Future.mapError(Error.fromSdkToString);

  let rename = (~settings, renaming) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.renameAliases(renaming)
    ->Future.mapError(Error.fromSdkToString);
};

module Accounts =
       (Getter: {let get: URL.t => Future.t(Result.t(Js.Json.t, string));}) => {
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

  let parse = content =>
    content
    ->Js.String2.split("\n")
    ->Array.map(row => row->Js.String2.split(":"))
    ->(rows => rows->Array.keep(data => data->Array.length >= 2))
    ->Array.map(pair => {
        [|pair->Array.getUnsafe(0)|]
        ->Array.concat(
            pair
            ->Array.getUnsafe(1)
            ->Js.String2.trim
            ->Js.String2.split(" ("),
          )
      })
    ->(rows => rows->Array.keep(data => data->Array.length > 2))
    ->Array.map(data => (data[0], data[1]));

  let get = (~settings: AppSettings.t) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapError(Error.fromSdkToString)
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

  let updateSecretAt = (secret, ~settings, index) =>
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

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let import = (~settings, key, name, ~password) => {
    let skUri = "encrypted:" ++ key;
    settings
    ->AppSettings.sdk
    ->TezosSDK.importSecretKey(~name, ~skUri, ~password, ())
    ->Future.mapError(Error.fromSdkToString)
    ->Future.tapOk(k => Js.log("key found : " ++ k));
  };

  let derive = (~settings, ~index, ~name, ~password) =>
    Future.mapOk2(
      secretAt(~settings, index),
      recoveryPhraseAt(~settings, index, ~password),
      (secret, recoveryPhrase) => {
      secret.derivationPath
      ->DerivationPath.Pattern.implement(secret.addresses->Array.length)
      ->HD.edesk(recoveryPhrase->HD.seed, ~password)
      ->Future.flatMapOk(edesk => import(~settings, edesk, name, ~password))
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
    ->Future.mapError(Error.fromSdkToString);

  let delete = (~settings, name) =>
    Aliases.getAddressForAlias(~settings, name)
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
          secret.addresses->Array.keepMap(aliases->Map.String.get)
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

  let validate = (network, address) => {
    network
    ->APICommon.Explorer.get(address, ~limit=1, ())
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
        import(~settings, edesk, name, ~password)
        ->Future.flatMapOk(address
            // always include 0'
            =>
              (
                index == 0
                  ? Future.value(Ok(true)) : settings->validate(address)
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
    ->Future.mapError(Error.fromSdkToString);

  let legacyScan = (~settings, recoveryPhrase, name, ~password) =>
    legacyImport(~settings, name, recoveryPhrase, ~password)
    ->Future.flatMapOk(legacyAddress =>
        settings
        ->validate(legacyAddress)
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
        backupPhrase,
        name,
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

  let unsafeDeleteAddresses = (~settings, addresses) =>
    Aliases.getAliasMap(~settings)
    ->Future.mapOk(aliases =>
        addresses->Array.keepMap(aliases->Map.String.get)
      )
    ->Future.flatMapOk(names =>
        names->Array.map(unsafeDelete(~settings))->FutureEx.all
      );

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

module DelegateMaker =
       (Get: {let get: URL.t => Future.t(Result.t(Js.Json.t, string));}) => {
  let parse = content =>
    if (content == "none\n") {
      None;
    } else {
      let splittedContent = content->Js.String2.split(" ");
      if (content->Js.String2.length == 0 || splittedContent->Array.length == 0) {
        None;
      } else {
        Some(splittedContent->Array.getUnsafe(0));
      };
    };

  let getForAccount = (settings, account) =>
    ReTaquito.getDelegate(settings->AppSettings.endpoint, account)
    ->Future.mapOk(result =>
        switch (result) {
        | Some(delegate) =>
          if (account == delegate) {
            None;
          } else {
            result;
          }
        | None => None
        }
      );

  let getBakers = (settings: AppSettings.t) =>
    switch (settings->AppSettings.network) {
    | `Mainnet =>
      URL.External.bakingBadBakers
      ->URL.get
      ->Future.mapOk(Json.Decode.(array(Delegate.decode)))
    | `Testnet(_) =>
      Future.value(
        Ok([|
          {name: "zebra", address: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"},
        |]),
      )
    };

  type delegationInfo = {
    initialBalance: Tez.t,
    delegate: string,
    timestamp: Js.Date.t,
    lastReward: option(Tez.t),
  };

  let getDelegationInfoForAccount =
      (network, account: string)
      : Future.t(Belt.Result.t(option(delegationInfo), Js.String.t)) => {
    module ExplorerAPI = ServerAPI.ExplorerMaker(Get);
    module BalanceAPI = Balance;
    network
    ->ExplorerAPI.get(account, ~types=[|"delegation"|], ~limit=1, ())
    ->Future.flatMapOk(operations =>
        if (operations->Array.length == 0) {
          Future.value(Ok(None));
        } else {
          let firstOperation = operations->Array.getUnsafe(0);
          switch (firstOperation.payload) {
          | Business(payload) =>
            switch (payload.payload) {
            | Delegation(payload) =>
              switch (payload.delegate) {
              | Some(delegate) =>
                if (account == delegate) {
                  Future.value(
                    Ok(
                      {
                        initialBalance: Tez.zero,
                        delegate: "",
                        timestamp: Js.Date.make(),
                        lastReward: None,
                      }
                      ->Some,
                    ),
                  );
                } else {
                  network
                  ->BalanceAPI.get(
                      account,
                      ~params={block: firstOperation.level->string_of_int},
                      (),
                    )
                  ->Future.mapOk(balance =>
                      {
                        initialBalance: balance,
                        delegate,
                        timestamp: firstOperation.timestamp,
                        lastReward: None,
                      }
                    )
                  ->Future.flatMapOk(info =>
                      network
                      ->ExplorerAPI.get(
                          info.delegate,
                          ~types=[|"transaction"|],
                          ~destination=account,
                          ~limit=1,
                          (),
                        )
                      ->Future.mapOk(operations =>
                          if (operations->Array.length == 0) {
                            info->Some;
                          } else {
                            switch (firstOperation.payload) {
                            | Business(payload) =>
                              switch (payload.payload) {
                              | Transaction(payload) =>
                                {...info, lastReward: Some(payload.amount)}
                                ->Some
                              | _ => info->Some
                              }
                            };
                          }
                        )
                    );
                }
              | None =>
                Js.log("No delegation set");
                Future.value(Ok(None));
              }
            | _ => Future.value(Error("Invalid operation type!"))
            }
          };
        }
      );
  };
};

module Delegate = DelegateMaker(URL);

module Tokens = {
  let checkTokenContract = (settings, contract) => {
    URL.Explorer.checkToken(settings, ~contract)
    ->URL.get
    ->Future.map(result => {
        switch (result) {
        | Ok(json) =>
          switch (Js.Json.classify(json)) {
          | Js.Json.JSONTrue => Ok(true)
          | JSONFalse => Ok(false)
          | _ => Error("Error")
          }
        | Error(e) => Error(e)
        }
      });
  };

  let batchEstimate = (settings, transfers, ~source, ~index=?, ()) =>
    Simulation.batch(settings, transfers, ~source, ~index?, ());

  let batch = (settings, transfers, ~source, ~password) =>
    Operation.batch(settings, transfers, ~source, ~password);

  let offline = (operation: Token.operation) => {
    switch (operation) {
    | Transfer(_)
    | Approve(_) => false
    | GetBalance(_)
    | GetAllowance(_)
    | GetTotalSupply(_) => true
    };
  };

  let simulate = (network, ~index=?, operation: Token.operation) =>
    switch (operation) {
    | Transfer({source, transfers, _}) =>
      batchEstimate(network, transfers, ~source, ~index?, ())
      ->Future.mapError(e => e->Error.Taquito)
    | _ =>
      Future.value(
        SimulationNotAvailable(Token.operationEntrypoint(operation))
        ->Error.token
        ->Error,
      )
    };

  let inject = (network, operation: Token.operation, ~password) =>
    switch (operation) {
    | Transfer({source, transfers, _}) =>
      batch(network, transfers, ~source, ~password)
      ->Future.mapError(Error.taquito)
    | _ =>
      Future.value(
        InjectionNotImplemented(Token.operationEntrypoint(operation))
        ->Error.token
        ->Error,
      )
    };

  let callGetOperationOffline = (settings, operation: Token.operation) =>
    if (offline(operation)) {
      switch (operation) {
      | GetBalance({token, address, _}) =>
        URL.Explorer.getTokenBalance(
          settings,
          ~contract=token,
          ~account=address,
        )
        ->URL.get
        ->Future.flatMapOk(res => {
            switch (res->Js.Json.decodeString) {
            | None => Token.Unit.zero->Ok->Future.value
            | Some(v) =>
              v
              ->Token.Unit.fromNatString
              ->FutureEx.fromOption(~error="cannot read Token amount: " ++ v)
            }
          })
        ->Future.mapError(s => s->RawError->Error.Token)
      | _ =>
        Future.value(
          OffchainCallNotImplemented(Token.operationEntrypoint(operation))
          ->Error.token
          ->Error,
        )
      };
    } else {
      Future.value(
        OperationNotRunnableOffchain(Token.operationEntrypoint(operation))
        ->Error.token
        ->Error,
      );
    };
};

module Signature = {
  let signPayload = (settings, ~source, ~password, ~payload) => {
    ReTaquito.Signature.signPayload(
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~password,
      ~payload,
    )
    ->Future.mapError(taquitoError => Error.Taquito(taquitoError));
  };
};
