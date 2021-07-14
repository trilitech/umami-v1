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
  let get = (~settings, ()) =>
    WalletAPI.Accounts.secrets(~settings)
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
  let get = (~settings, password) =>
    WalletAPI.Accounts.recoveryPhraseAt(~settings, index, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Account, ~setRequest, ());

  (request, getRequest);
};

let useScanGlobal = (~requestState as (request, setRequest), ()) => {
  let get = (~settings, password) =>
    WalletAPI.Accounts.scanAll(~settings, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Account, ~setRequest, ());

  (request, getRequest);
};

/* Set */

type deriveInput = {
  name: string,
  index: int,
  password: string,
};

let useDerive =
  ApiRequest.useSetter(
    ~set=
      (~settings, {name, index, password}) =>
        WalletAPI.Accounts.derive(~settings, ~index, ~alias=name, ~password),
    ~kind=Logs.Account,
  );

type createInput = {
  name: string,
  mnemonics: string,
  derivationPath: DerivationPath.Pattern.t,
  password: string,
};

let useCreateWithMnemonics =
  ApiRequest.useSetter(
    ~set=
      (~settings, {name, mnemonics, derivationPath, password}) =>
        WalletAPI.Accounts.restore(
          ~settings,
          ~backupPhrase=mnemonics,
          ~name,
          ~derivationPath,
          ~password,
          (),
        ),
    ~kind=Logs.Account,
  );

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (~settings, {index, secret}: Secret.derived) => {
        WalletAPI.Accounts.updateSecretAt(~settings, secret, index)
      },
    ~kind=Logs.Account,
  );

let useDelete =
  ApiRequest.useSetter(
    ~set=WalletAPI.Accounts.deleteSecretAt,
    ~kind=Logs.Account,
  );
