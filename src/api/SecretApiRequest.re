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
    ->Result.mapWithDefault(Future.value(Result.Ok([||])), secrets => {
        Future.value(Result.Ok(secrets))
      })
    ->Future.mapOk(secrets =>
        secrets->Array.mapWithIndex((index, secret) =>
          Secret.{index, secret}
        )
      );

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState, ());
};

let useGetRecoveryPhrase = (~requestState as (request, setRequest), ~index) => {
  let get = (~config, password) =>
    WalletAPI.Accounts.recoveryPhraseAt(~config, index, ~password);

  let getRequest =
    ApiRequest.useGetter(
      ~get,
      ~kind=Logs.Account,
      ~setRequest,
      ~errorToString=ErrorHandler.toString,
      (),
    );

  (request, getRequest);
};

let useScanGlobal = (~requestState as (request, setRequest), ()) => {
  let get = (~config, password) =>
    WalletAPI.Accounts.Scan.runAll(~config, ~password);

  let getRequest =
    ApiRequest.useGetter(
      ~get,
      ~kind=Logs.Account,
      ~setRequest,
      ~errorToString=ErrorHandler.toString,
      (),
    );

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

let useDerive =
  ApiRequest.useSetter(
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
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
  );

type createInput = {
  name: string,
  mnemonic: array(string),
  derivationPath: DerivationPath.Pattern.t,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    ~set=
      (~config, {name, mnemonic, derivationPath, password}) =>
        WalletAPI.Accounts.restore(
          ~config,
          ~backupPhrase=mnemonic,
          ~name,
          ~derivationPath,
          ~password,
          (),
        ),
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
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
    ~errorToString=ErrorHandler.toString,
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
    ~kind=Logs.Account,
  );

type ledgerScanInput = {
  index: int,
  accountsNumber: int,
  ledgerMasterKey: PublicKeyHash.t,
  timeout: int,
};

let useLedgerScan =
  ApiRequest.useSetter(
    ~errorToString=ErrorHandler.toString,
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
    ~kind=Logs.Account,
  );

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~config, {index, secret}: Secret.derived) => {
        WalletAPI.Accounts.updateSecretAt(~config, secret, index)
      },
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
  );

let useDelete =
  ApiRequest.useSetter(
    ~set=WalletAPI.Accounts.deleteSecretAt,
    ~kind=Logs.Account,
    ~errorToString=ErrorHandler.toString,
  );
