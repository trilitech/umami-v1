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

type spsk = pri string;

type t;

type network = [ | `mainnet | `testnet];

[@bs.deriving abstract]
type makeParams = {
  baseUrl: string,
  [@bs.optional]
  redirectPathName: string,
  [@bs.optional]
  redirectToOpener: bool,
  [@bs.optional]
  enableLogging: bool,
  network,
};

[@bs.deriving abstract]
type initParams = {
  [@bs.optional]
  skipSw: bool,
};

type provider = [ | `google];

let providerToString: provider => string =
  fun
  | `google => "google";

type Errors.t +=
  | UnableToRetrieveHandle
  | InvalidProvider(string);

let () =
  Errors.registerHandler(
    "CustomAuth",
    fun
    | InvalidProvider(s) => I18n.Errors.invalid_provider(s)->Some
    | UnableToRetrieveHandle =>
      I18n.Errors.customauth_unable_to_retrieve_handle->Some
    | _ => None,
  );

let providerFromString: string => Let.result(provider) =
  fun
  | "google" => `google->Ok
  | s => Error(InvalidProvider(s));

module Handle: {
  type t = pri string;
  let fromString: string => t;
  let display: (t, provider) => string;

  let resolve: (option(t), option(t)) => Let.result(t);
} = {
  type t = string;
  let fromString = x => x;

  let display = (name: t, provider) =>
    switch (provider) {
    | `google => name
    };

  let resolve = (email, name) =>
    switch (email, name) {
    | (None | Some(""), None | Some("")) => Error(UnableToRetrieveHandle)
    | (None | Some(""), Some(name)) => Ok(name)
    | (Some(email), _) => Ok(email)
    };
};

type prompt = [ | `none | `login | `consent | `select_account];
type display = [ | `page | `popup | `touch | `wap];

[@bs.deriving abstract]
type jwtParams = {
  [@bs.optional]
  domain: string,
  [@bs.optional]
  login_hint: Handle.t,
  [@bs.optional]
  prompt,
  [@bs.optional]
  display,
};

[@bs.deriving abstract]
type triggerLoginParams = {
  typeOfLogin: provider,
  verifier: string,
  clientId: string,
  [@bs.optional]
  jwtParams,
};

[@bs.deriving abstract]
type subVerifier = {
  clientId: string,
  typeOfLogin: provider,
  verifier: string,
  [@bs.optional]
  jwtParams,
};

type aggregateVerifier = [ | `single_id_verifier];

[@bs.deriving abstract]
type triggerAggregateLoginParams = {
  aggregateVerifierType: aggregateVerifier,
  verifierIdentifier: string,
  subVerifierDetailsArray: array(subVerifier),
};

type userInfo = {
  email: option(Handle.t),
  name: option(Handle.t),
  typeOfLogin: provider,
};

type aggregateLoginDetails = {
  privateKey: spsk,
  publicKey: string,
  userInfo: array(userInfo),
};

let findInfo = (infos, provider) =>
  infos
  ->Array.getBy(i => i.typeOfLogin == provider)
  ->Result.fromOption(UnableToRetrieveHandle);

type infos = {
  provider,
  handle: Handle.t,
};

module Msg = {
  type instanceParams = {
    instanceId: string,
    verifier: string,
    typeOfLogin: string,
    redirectToOpener: bool,
  };

  type hashParams = {
    state: string,
    access_token: string,
    token_type: string,
    expires_in: string,
    scope: string,
    id_token: string,
    authuser: string,
    hd: string,
    prompt: string,
  };

  type data = {
    instanceParams,
    hashParams,
  };

  type t = {
    channel: string,
    data,
    error: string,
  };
};
