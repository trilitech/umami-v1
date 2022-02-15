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

let rebuildMessageFromUrl = url => {
  open ReCustomAuthType.Msg;
  open URLSearchParams;
  let url = make(url);

  let getString = (v, k) => v->getString(k)->Option.getExn;
  let getBool = (v, k) => v->getBool(k)->Option.getExn;

  let instanceParams = {
    instanceId: url->getString("instanceId"),
    verifier: url->getString("verifier"),
    typeOfLogin: url->getString("typeOfLogin"),
    redirectToOpener: url->getBool("redirectToOpener"),
  };

  let hashParams = {
    state: url->getString("state"),
    access_token: url->getString("access_token"),
    token_type: url->getString("token_type"),
    expires_in: url->getString("expires_in"),
    scope: url->getString("scope"),
    id_token: url->getString("id_token"),
    authuser: url->getString("authuser"),
    hd: url->getString("hd"),
    prompt: url->getString("prompt"),
  };

  let data = {instanceParams, hashParams};

  let t = {
    channel: url->getString("channel"),
    data,
    error: url->getString("error"),
  };
  t;
};

[@bs.scope "window"] [@bs.val]
external postMessage: ReCustomAuthType.Msg.t => unit = "postMessage";

let authPrefix = "auth/";

/* Handles auth message send from [redirect.html] through deeplink.
   [redirect.html] uses [window.open] and POST data is not allowed through
   this function. Hence, data is send through GET as url parameters.
   This function parses URL parameters and convert it back to [Msg.t]
   and broadcast it to CustomAuth sdk through [postMessage].
   */
let useDeeplinkHandler = () => {
  let deeplink = IPC.useDeeplink();

  React.useEffect1(
    () => {
      switch (deeplink) {
      | Some(msg) when msg->Js.String2.startsWith(authPrefix) =>
        msg
        ->Js.String2.substr(~from=authPrefix->Js.String2.length)
        ->rebuildMessageFromUrl
        ->postMessage
      | Some(_)
      | None => ()
      };

      None;
    },
    [|deeplink|],
  );
};
