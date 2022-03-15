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

open Let;

let makeClient = () =>
  ReBeacon.WalletClient.make({name: I18n.Label.beacon_client_name});

let dataFromURL = url => {
  URL.make(url)
  |> URL.getSearchParams
  |> URL.SearchParams.get("data")
  |> Js.Nullable.toOption;
};

type Errors.t +=
  | ClientNotConnected
  | NoDeeplink
  | OperationNotSupported
  | BeaconNotHandled
  | NetworkMismatch;

let () =
  Errors.registerHandler(
    "BeaconAPI",
    fun
    | ClientNotConnected => I18n.Errors.beacon_client_not_created->Some
    | NoDeeplink => I18n.Errors.deeplinking_not_connected->Some
    | NetworkMismatch => I18n.Errors.beacon_request_network_missmatch->Some
    | OperationNotSupported => I18n.Errors.beacon_operation_not_supported->Some
    | BeaconNotHandled => I18n.Errors.beacon_cant_handle->Some
    | _ => None,
  );

open ReBeacon.Message.Request;
let requestToBatch = (account, {operationDetails}) => {
  let managers =
    operationDetails->Array.map(o =>
      switch (ReBeacon.Message.Request.PartialOperation.classify(o)) {
      | Origination(orig) =>
        ProtocolHelper.Origination.make(
          ~balance=Tez.fromMutezString(orig.balance),
          ~code=orig.script.code,
          ~storage=orig.script.storage,
          ~delegate=orig.delegate,
          (),
        )
        ->Ok

      | Transfer({destination, amount, parameters}) =>
        ProtocolHelper.Transfer.makeSimple(
          ~data={destination, amount: Tez(Tez.fromMutezString(amount))},
          ~parameter=?parameters->Option.map(a => a.value),
          ~entrypoint=?parameters->Option.map(a => a.entrypoint),
          (),
        )
        ->Protocol.Transfer
        ->Ok

      | Delegation({delegate}) =>
        let delegate =
          switch (delegate) {
          | Some(d) => Protocol.Delegation.Delegate(d)
          | None => Undelegate(None)
          };

        {delegate, options: ProtocolOptions.make()}->Protocol.Delegation->Ok;

      | _ => Error(OperationNotSupported)
      }
    );

  let%ResMap managers = managers->Result.collectArray;

  Protocol.{source: account, managers};
};

let useNextRequestState = (client, peersRequestState) => {
  let (client, _) = client;
  let (_, setPeersRequest) = peersRequestState;

  let (nextRequest, doneResponding, yield) = ReactUtils.useNextState();
  let (isConnected, setIsConnected) = React.useState(() => false);

  let (nextDeeplink, doneDeeplinking) = IPC.useNextDeeplinkState();

  ReactUtils.useAsyncEffect1(
    () => {
      let%Await client =
        client->Promise.fromOption(~error=ClientNotConnected);
      let%Await _: ReBeacon.transportType = client->ReBeacon.WalletClient.init;
      let%AwaitMap () =
        client->ReBeacon.WalletClient.connect(message => {
          let request = message->ReBeacon.Message.Request.classify;
          yield(request);
        });
      setIsConnected(_ => true);
    }, // what is the expected behavior when beacon fails to connect?;
    [|client|],
  );

  React.useEffect2(
    () => {
      if (isConnected) {
        Promise.async(() => {
          let%Await deeplink =
            nextDeeplink()->Promise.fromOption(~error=NoDeeplink);
          let%Await peer =
            ReBeacon.Serializer.(
              make()
              ->deserialize(deeplink->dataFromURL->Option.getWithDefault(""))
            );
          let%Await client =
            client->Promise.fromOption(~error=ClientNotConnected);
          let%AwaitMap () = client->ReBeacon.WalletClient.addPeer(peer);

          setPeersRequest(ApiRequest.expireCache);
          doneDeeplinking();
        });
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
    let get = (~config as _s, ()) => {
      let%Await client =
        client->Promise.fromOption(~error=ClientNotConnected);
      client->ReBeacon.WalletClient.getPeers;
    };

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ());
  };

  let useDelete = (~client) => {
    ApiRequest.useSetter(
      ~set=
        (~config as _, peer: ReBeacon.peerInfo) => {
          let%Await client =
            client->Promise.fromOption(~error=ClientNotConnected);
          client->ReBeacon.WalletClient.removePeer(peer);
        },
      ~kind=Logs.Beacon,
    );
  };
};

/* PERMISSIONS */

module Permissions = {
  let useLoad = (client, requestState) => {
    let get = (~config as _s, ()) => {
      let%Await client =
        client->Promise.fromOption(~error=ClientNotConnected);

      client->ReBeacon.WalletClient.getPermissions;
    };

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ());
  };

  let useDelete = (~client) => {
    ApiRequest.useSetter(
      ~set=
        (~config as _s, accountIdentifier: ReBeacon.accountIdentifier) => {
          let%Await client =
            client->Promise.fromOption(~error=ClientNotConnected);

          client->ReBeacon.WalletClient.removePermission(accountIdentifier);
        },
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
