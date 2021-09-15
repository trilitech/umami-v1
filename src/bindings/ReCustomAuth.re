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

let network = `testnet;

/* An auth provider redirects to a web page owned by the
   auth consumer (us/our api) to ensure only this consumer's
   users can use it's auth api key.

   After authentication is done, the provider opens the following url
   where `redirect.html`, (implemented by CustomAuth) is stored.

   This page calls for the parent browser window (the opener = umami)
   and CustomAuth receives the authentication informations
   */
let redirectDomain = "https://umamiwallet.com";
let baseUrl = redirectDomain ++ "/auth/";
let redirectPathName = "redirect.html";

include ReCustomAuthType;

[@bs.module "@toruslabs/torus-direct-web-sdk"] [@bs.new]
external make: makeParams => t = "default";

[@bs.send] external init: (t, initParams) => Js.Promise.t(unit) = "init";

[@bs.send]
external triggerAggregateLogin:
  (t, triggerAggregateLoginParams) => Js.Promise.t(aggregateLoginDetails) =
  "triggerAggregateLogin";

let torusSdk =
  make(
    makeParams(
      ~baseUrl,
      ~redirectPathName,
      ~redirectToOpener=true,
      ~network,
      (),
    ),
  );

type Errors.t +=
  | AccessDenied
  | UserClosedPopup;

let () =
  Errors.registerHandler(
    "CustomAuth",
    fun
    | UserClosedPopup => I18n.Errors.customauth_popup_closed->Some
    | AccessDenied => I18n.Errors.customauth_unable_to_retrieve_handle->Some
    | _ => None,
  );

let user_closed_popup = "user closed popup";
let access_denied = "access_denied";

let parse = (e: RawJsError.t) =>
  switch (e.message) {
  | s when s->Js.String2.includes(user_closed_popup) => UserClosedPopup
  | s when s->Js.String2.includes(access_denied) => AccessDenied
  | s => Errors.Generic(Js.String.make(s))
  };

/* Initiate the sdk by checking for access to torus and redirection page
   https://docs.tor.us/customauth/api-reference/initialization
   */
let init = (sdk, initParams) => {
  sdk->init(initParams)->RawJsError.fromPromiseParsed(parse);
};

/* [triggerAggregateLogin] runs the following steps:
   - opens a auth provider pop
   - receives the auth information
   - fetches the secret key from Torus
   */
let triggerAggregateLogin = (sdk, params) => {
  sdk->triggerAggregateLogin(params)->RawJsError.fromPromiseParsed(parse);
};
