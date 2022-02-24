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

/* ACCOUNT */

/* Get */

let useLoad = requestState => {
  let get = (~config, ()) =>
    WalletAPI.Accounts.secrets(~config)
    ->Result.mapWithDefault(Promise.ok([||]), secrets => {
        Promise.value(Result.Ok(secrets))
      })
    ->Promise.mapOk(secrets =>
        secrets->Array.mapWithIndex((index, secret) =>
          Secret.{index, secret}
        )
      );

  ApiRequest.useLoader(~get, ~kind=Logs.Secret, ~requestState, ());
};

let useGetRecoveryPhrase = (~requestState as (request, setRequest), ~index) => {
  let get = (~config as _, password) =>
    WalletAPI.Accounts.recoveryPhraseAt(index, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Secret, ~setRequest, ());

  (request, getRequest);
};

/* Set */

type deriveKind =
  | Mnemonics(string) // password
  | Ledger(PublicKeyHash.t); // Ledger Master Key;

type deriveInput = {
  name: string,
  index: int,
  kind: deriveKind,
  timeout: option(int),
};

let keepNonFormErrors =
  fun
  | SecureStorage.Cipher.DecryptError => false
  | _ => true;

let useDerive =
  ApiRequest.useSetter(
    ~keepError=keepNonFormErrors,
    ~logOk=_ => I18n.account_created,
    ~set=
      (~config, {name, index, kind, timeout}) =>
        switch (kind) {
        | Mnemonics(password) =>
          WalletAPI.Accounts.derive(~config, ~index, ~alias=name, ~password)
        | Ledger(ledgerMasterKey) =>
          WalletAPI.Accounts.deriveLedger(
            ~config,
            ~timeout?,
            ~index,
            ~alias=name,
            ~ledgerMasterKey,
            (),
          )
        },
    ~kind=Logs.Secret,
  );

type createInput = {
  name: string,
  mnemonic: array(string),
  derivationPath: DerivationPath.Pattern.t,
  password: string,
  backupFile: option(System.Path.t),
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    ~logOk=_ => I18n.account_created,
    ~keepError=keepNonFormErrors,
    ~set=
      (~config, {name, mnemonic, derivationPath, password, backupFile}) => {
        // The condition assumes there's no backupFile already configured
        // and one is given explicitely
        let config = {
          ...config,
          backupFile:
            config.backupFile == None && backupFile != None
              ? backupFile : config.backupFile,
        };
        WalletAPI.Accounts.restore(
          ~config,
          ~backupPhrase=mnemonic,
          ~name,
          ~derivationPath,
          ~password,
          (),
        );
      },
    ~kind=Logs.Secret,
  );

type createFromBackupInput = {
  backupFile: System.Path.t,
  password: string,
};

let useCreateFromBackupFile =
  ApiRequest.useSetter(
    ~logOk=_ => I18n.account_created,
    ~keepError=keepNonFormErrors,
    ~set=
      (~config, {backupFile, password}) => {
        WalletAPI.Accounts.restoreFromBackupFile(
          ~config,
          ~backupFile,
          ~password,
          (),
        )
      },
    ~kind=Logs.Secret,
  );

type ledgerImportInput = {
  name: string,
  derivationPath: DerivationPath.Pattern.t,
  derivationScheme: Wallet.Ledger.scheme,
  accountsNumber: int,
  ledgerMasterKey: PublicKeyHash.t,
  timeout: int,
};

let useLedgerImport =
  ApiRequest.useSetter(
    ~set=
      (
        ~config,
        {
          name,
          derivationPath,
          derivationScheme,
          accountsNumber,
          ledgerMasterKey,
          timeout,
        },
      ) =>
        WalletAPI.Accounts.importLedger(
          ~config,
          ~name,
          ~accountsNumber,
          ~derivationPath,
          ~derivationScheme,
          ~ledgerMasterKey,
          ~timeout,
          (),
        ),
    ~kind=Logs.Secret,
  );

type ledgerScanInput = {
  index: int,
  accountsNumber: int,
  ledgerMasterKey: PublicKeyHash.t,
  timeout: int,
};

let useLedgerScan =
  ApiRequest.useSetter(
    ~set=
      (~config, {index, accountsNumber, ledgerMasterKey, timeout}) =>
        WalletAPI.Accounts.deriveLedgerKeys(
          ~config,
          ~index,
          ~accountsNumber,
          ~ledgerMasterKey,
          ~timeout,
          (),
        ),
    ~kind=Logs.Secret,
  );

type account = WalletAPI.Accounts.Scan.account(string);

type mnemonicImportKeysInput = {
  index: int,
  accounts: list(account),
  password: string,
};

let useMnemonicImportKeys =
  ApiRequest.useSetter(
    ~set=
      (~config, {index, accounts, password}) =>
        WalletAPI.Accounts.importMnemonicKeys(
          ~config,
          ~index,
          ~accounts,
          ~password,
          (),
        ),
    ~kind=Logs.Secret,
  );

let useUpdate =
  ApiRequest.useSetter(
    ~logOk=_ => I18n.secret_updated,
    ~set=
      (~config, {index, secret}: Secret.derived) => {
        WalletAPI.Accounts.updateSecretAt(~config, secret, index)
        ->Promise.value
      },
    ~kind=Logs.Secret,
  );

let useDelete =
  ApiRequest.useSetter(
    ~logOk=_ => I18n.secret_deleted,
    ~set=WalletAPI.Accounts.deleteSecretAt,
    ~kind=Logs.Secret,
  );
