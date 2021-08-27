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

open UmamiCommon;

let makeClient = () => ReBeacon.WalletClient.make({name: "Umami"});

let dataFromURL = url => {
  URL.make(url)
  |> URL.getSearchParams
  |> URL.SearchParams.get("data")
  |> Js.Nullable.toOption;
};

let useNextRequestState = (client, peersRequestState) => {
  let (client, _) = client;
  let (_, setPeersRequest) = peersRequestState;

  let (nextRequest, doneResponding, yield) = ReactUtils.useNextState();
  let (isConnected, setIsConnected) = React.useState(() => false);

  let (nextDeeplink, doneDeeplinking) = IPC.useNextDeeplinkState();

  React.useEffect1(
    () => {
      client
      ->ReBeacon.WalletClient.init
      ->Future.flatMapOk(_ =>
          client->ReBeacon.WalletClient.connect(message => {
            let request = message->ReBeacon.Message.Request.classify;
            yield(request);
          })
        )
      ->FutureEx.getOk(_ => setIsConnected(_ => true)); // what is the expected behavior when beacon fails to connect?
      None;
    },
    [|client|],
  );

  React.useEffect2(
    () => {
      if (isConnected) {
        nextDeeplink()
        ->Lib.Option.iter(deeplink =>
            ReBeacon.Serializer.(
              make()
              ->deserialize(deeplink->dataFromURL->Option.getWithDefault(""))
            )
            ->Future.flatMapOk(peer =>
                client
                ->ReBeacon.WalletClient.addPeer(peer)
                ->Future.tapOk(_ => setPeersRequest(ApiRequest.expireCache))
              )
            ->Future.get(_ => doneDeeplinking())
          );
      };
      None;
    },
    (isConnected, nextDeeplink),
  );

  (nextRequest, doneResponding);
};

/* PEERS */

module Peers = {
  let useLoad = (client, requestState) => {
    let get = (~config as _s, ()) => client->ReBeacon.WalletClient.getPeers;

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ());
  };

  let useDelete = (~client) => {
    ApiRequest.useSetter(
      ~set=
        (~config as _s, peer: ReBeacon.peerInfo) =>
          client->ReBeacon.WalletClient.removePeer(peer),
      ~kind=Logs.Beacon,
    );
  };
};

/* PERMISSIONS */

module Permissions = {
  let useLoad = (client, requestState) => {
    let get = (~config as _s, ()) =>
      client->ReBeacon.WalletClient.getPermissions;

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ());
  };

  let useDelete = (~client) => {
    ApiRequest.useSetter(
      ~set=
        (~config as _s, accountIdentifier: ReBeacon.accountIdentifier) =>
          client->ReBeacon.WalletClient.removePermission(accountIdentifier),
      ~kind=Logs.Beacon,
    );
  };
};

/* SIGNATURE */

module Signature = {
  let useSignPayload = () => {
    let config = ConfigContext.useContent();

    (~source, ~signingIntent, ~payload) =>
      NodeAPI.Signature.signPayload(
        config,
        ~source,
        ~signingIntent,
        ~payload,
      );
  };
};
