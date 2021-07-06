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

module AccountsAPI = AccountApiRequest.AccountsAPI;

/* Get */

let useLoad = requestState => {
  let get = (~settings, ()) =>
    AccountsAPI.secrets(~settings)
    ->Option.mapWithDefault(Future.value(Result.Ok([||])), secrets => {
        Future.value(Result.Ok(secrets))
      })
    ->Future.mapOk(secrets =>
        secrets->Array.mapWithIndex(
          (index, {name, derivationPath, addresses, legacyAddress}) =>
          Secret.{index, name, derivationPath, addresses, legacyAddress}
        )
      );

  ApiRequest.useLoader(~get, ~kind=Logs.Account, ~requestState, ());
};

let useGetRecoveryPhrase = (~requestState as (request, setRequest), ~index) => {
  let get = (~settings, password) =>
    AccountsAPI.recoveryPhraseAt(~settings, index, ~password);

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Account, ~setRequest, ());

  (request, getRequest);
};

let useScanGlobal = (~requestState as (request, setRequest), ()) => {
  let get = (~settings, password) =>
    AccountsAPI.scanAll(~settings, ~password);

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
        AccountsAPI.derive(~settings, ~index, ~name, ~password),
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
        AccountsAPI.restore(
          ~settings,
          mnemonics,
          name,
          ~derivationPath,
          ~password,
          (),
        ),
    ~kind=Logs.Account,
  );

let useUpdate =
  ApiRequest.useSetter(
    ~set=
      (
        ~settings,
        {index, name, derivationPath, addresses, legacyAddress}: Secret.t,
      ) => {
        let secret =
          API.Secret.{name, derivationPath, addresses, legacyAddress};
        AccountsAPI.updateSecretAt(secret, ~settings, index);
      },
    ~kind=Logs.Account,
  );

let useDelete =
  ApiRequest.useSetter(~set=AccountsAPI.deleteSecretAt, ~kind=Logs.Account);
