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

let client = ReBeacon.WalletClient.make({name: "umami"});

/* PEERS */

module Peers = {
  let useLoad = requestState => {
    let get = (~settings as _s, ()) =>
      client
      ->ReBeacon.WalletClient.getPeers
      ->FutureJs.fromPromise(Js.String.make);

    ApiRequest.useLoader(~get, ~kind=Logs.Settings, ~requestState, ());
  };

  let useDelete =
    ApiRequest.useSetter(
      ~set=
        (~settings as _s, peer: ReBeacon.peerInfo) =>
          client
          ->ReBeacon.WalletClient.removePeer(peer)
          ->FutureJs.fromPromise(Js.String.make),
      ~kind=Logs.Settings,
    );
};

/* PERMISSIONS */

module Permissions = {
  let useLoad = requestState => {
    let get = (~settings as _s, ()) =>
      client
      ->ReBeacon.WalletClient.getPermissions
      ->FutureJs.fromPromise(Js.String.make);

    ApiRequest.useLoader(~get, ~kind=Logs.Settings, ~requestState, ());
  };

  let useDelete =
    ApiRequest.useSetter(
      ~set=
        (~settings as _s, accountIdentifier: ReBeacon.accountIdentifier) =>
          client
          ->ReBeacon.WalletClient.removePermission(accountIdentifier)
          ->FutureJs.fromPromise(Js.String.make),
      ~kind=Logs.Settings,
    );
};

/* SIGNATURE */

module Signature = {
  let useSignPayload = () => {
    let settings = SdkContext.useSettings();

    (~source, ~password, ~payload) =>
      API.Signature.signPayload(settings, ~source, ~password, ~payload);
  };
};
