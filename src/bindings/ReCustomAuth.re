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

let flagOn = true;

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
let network = `mainnet;

include ReCustomAuthType;

[@bs.module "@toruslabs/customauth"] [@bs.new]
external make: makeParams => t = "default";

[@bs.send] external init: (t, initParams) => Js.Promise.t(unit) = "init";
/*  */
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

module NodeDetails = {
  type endpoints;
  type nodePub;

  type default;

  type t = {
    [@bs.as "torusNodeEndpoints"]
    endpoints,
    [@bs.as "torusNodePub"]
    nodePub,
  };

  type params = {
    network: string,
    proxyAddress: string,
  };

  let mainnet = {
    network: "mainnet",
    proxyAddress: "0x638646503746d5456209e33a2ff5e3226d698bea",
  };

  let testnet = {
    network: "ropsten",
    proxyAddress: "0x4023d2a0D330bF11426B12C6144Cfb96B7fa6183",
  };

  [@bs.module "@toruslabs/fetch-node-details"] [@bs.new]
  external make: params => default = "default";

  [@bs.send] external get: default => Js.Promise.t(t) = "getNodeDetails";

  let get = () =>
    make(network == `mainnet ? mainnet : testnet)
    ->get
    ->RawJsError.fromPromiseParsed(parse);
};

module Utils = {
  type default;

  type address = {
    address: string,
    [@bs.as "X"]
    x: string,
    [@bs.as "Y"]
    y: string,
  };

  [@bs.module "@toruslabs/torus.js"] [@bs.new]
  external make: unit => default = "default";

  [@bs.send]
  external getPublicAddress:
    (
      default,
      NodeDetails.endpoints,
      NodeDetails.nodePub,
      lookupInfos,
      [@bs.as "true"] _
    ) =>
    Js.Promise.t(address) =
    "getPublicAddress";

  let getPublicAddress = (~verifier, handle) => {
    NodeDetails.get()
    ->Promise.flatMapOk((NodeDetails.{endpoints, nodePub}) => {
        let utils = make();
        utils
        ->getPublicAddress(
            endpoints,
            nodePub,
            {verifier, verifierId: handle},
          )
        ->RawJsError.fromPromiseParsed(parse);
      });
  };
};

/* Initiate the sdk by checking for access to torus and redirection page
     https://docs.tor.us/customauth/api-reference/initialization
   */
let init = (sdk, initParams) => {
  sdk->init(initParams)->RawJsError.fromPromiseParsed(parse);
};

/* [triggerAggregateLogin] runs the following steps:
   - opens a auth provider page on the external browser
   - receives the auth information
   - fetches the secret key from Torus

   auth informations are received from deeplink call from the browser
   see [useDeeplinkHandler] in module [ReCustomAuthUtils]
   */
let triggerAggregateLogin = (sdk, ~accountHandle=?, provider: handledProvider) => {
  let params =
    switch (provider) {
    | `google => CustomAuthVerifiers.googleParams(accountHandle)
    };

  sdk->triggerAggregateLogin(params)->RawJsError.fromPromiseParsed(parse);
};

let getPublicAddress = (~provider, handle) => {
  let verifier =
    switch (provider) {
    | `google => CustomAuthVerifiers.google
    };

  Utils.getPublicAddress(~verifier, handle)
  ->Promise.flatMapOk(({x, y}) => {
      Crypto.spPointsToPkh(~x, ~y)->Promise.value
    });
};
