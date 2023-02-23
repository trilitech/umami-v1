/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

let makeClient = () => ReBeacon.WalletClient.make({name: I18n.Label.beacon_client_name})

let dataFromURL = url =>
  URL.make(url)->URL.getSearchParams->URL.SearchParams.get("data")->Js.Nullable.toOption

type Errors.t +=
  | ClientNotConnected
  | NoDeeplink
  | OperationNotSupported
  | BeaconNotHandled
  | NetworkMismatch

let () = Errors.registerHandler("BeaconAPI", x =>
  switch x {
  | ClientNotConnected => I18n.Errors.beacon_client_not_created->Some
  | NoDeeplink => I18n.Errors.deeplinking_not_connected->Some
  | NetworkMismatch => I18n.Errors.beacon_request_network_missmatch->Some
  | OperationNotSupported => I18n.Errors.beacon_operation_not_supported->Some
  | BeaconNotHandled => I18n.Errors.beacon_cant_handle->Some
  | _ => None
  }
)

@ocaml.doc(
  " Cast Beacon.MichelineMichelsonV1Expression.t into ReTaquitoTypes.MichelsonV1Expression.t "
)
external // This is okay, because:
//
// Beacon:
// export type MichelineMichelsonV1Expression =
//   | { int: string }
//   | { string: string } // eslint-disable-line id-blacklist
//   | { bytes: string }
//   | MichelineMichelsonV1Expression[]
//   | {
//       prim: MichelsonPrimitives
//       args?: MichelineMichelsonV1Expression[]
//       annots?: string[]
//     }
//
// Taquito
// export interface MichelsonV1ExpressionBase {
//   int?: string;
//   string?: string;
//   bytes?: string;
// }
// export interface MichelsonV1ExpressionExtended {
//   prim: string;
//   args?: MichelsonV1Expression[];
//   annots?: string[];
// }
// export type MichelsonV1Expression = MichelsonV1ExpressionBase | MichelsonV1ExpressionExtended | MichelsonV1Expression[];
beaconToTaquito: Beacon.MichelineMichelsonV1Expression.t => ReTaquitoTypes.MichelsonV1Expression.t =
  "%identity"

open Beacon.Message.Request
let requestToBatch = ({operationDetails}) => {
  let managers = operationDetails->Array.map(o =>
    switch Beacon.Message.Request.PartialOperation.classify(o) {
    | Origination(orig) =>
      ProtocolHelper.Origination.make(
        ~balance=Tez.fromMutezString(orig.balance),
        ~code=orig.script.code,
        ~storage=orig.script.storage,
        ~delegate=orig.delegate->PublicKeyHash.unsafeBuild->Some,
        (),
      )->Ok

    | Transfer({destination, amount, parameters}) =>
      ProtocolHelper.Transfer.makeSimpleTez(
        ~destination=destination->PublicKeyHash.unsafeBuild,
        ~amount=Tez.fromMutezString(amount),
        ~parameter=?parameters->Option.map(a => a.value)->Option.map(beaconToTaquito),
        ~entrypoint=?parameters->Option.map(a => a.entrypoint),
        (),
      )
      ->Protocol.Transfer
      ->Ok

    | Delegation({delegate}) =>
      let delegate = switch delegate {
      | Some(d) => Protocol.Delegation.Delegate(d->PublicKeyHash.unsafeBuild)
      | None => Undelegate(None)
      }

      {delegate: delegate, options: ProtocolOptions.make()}->Protocol.Delegation->Ok

    | _ => Error(OperationNotSupported)
    }
  )

  managers->Result.collectArray
}

let useNextRequestState = (client, peersRequestState) => {
  let (client, _) = client
  let (_, setPeersRequest) = peersRequestState

  let (nextRequest, doneResponding, yield) = ReactUtils.useNextState()
  let (isConnected, setIsConnected) = React.useState(() => false)

  let (nextDeeplink, doneDeeplinking) = IPC.useNextDeeplinkState()

  ReactUtils.useAsyncEffect2(() =>
    client
    ->Promise.fromOption(~error=ClientNotConnected)
    ->Promise.flatMapOk(client => client->ReBeacon.WalletClient.init->Promise.mapOk(_ => client))
    ->Promise.flatMapOk(client =>
      client
      ->ReBeacon.WalletClient.connect(message => {
        let request = message->Beacon.Message.Request.classify
        yield(request)
      })
      ->Promise.mapOk(() => setIsConnected(_ => true))
    )
  , (client, yield)) // what is the expected behavior when beacon fails to connect?;

  React.useEffect2(() => {
    if isConnected {
      Promise.async(() =>
        nextDeeplink()
        ->Promise.fromOption(~error=NoDeeplink)
        ->Promise.flatMapOk(deeplink => {
          open ReBeacon.Serializer
          make()->deserialize(deeplink->dataFromURL->Option.getWithDefault(""))
        })
        ->Promise.flatMapOk(peer =>
          client
          ->Promise.fromOption(~error=ClientNotConnected)
          ->Promise.flatMapOk(client => client->ReBeacon.WalletClient.addPeer(peer))
          ->Promise.mapOk(_ => {
            setPeersRequest(ApiRequest.expireCache)
            doneDeeplinking()
          })
        )
      )
    }
    None
  }, (isConnected, nextDeeplink))

  (nextRequest, doneResponding)
}

/* PEERS */

module Peers = {
  let useLoad = (client, requestState) => {
    let get = (~config as _s, ()) =>
      client
      ->Promise.fromOption(~error=ClientNotConnected)
      ->Promise.flatMapOk(ReBeacon.WalletClient.getPeers)

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ())
  }

  let useDelete = (~client) =>
    ApiRequest.useSetter(
      ~set=(~config as _, peer: Beacon.peerInfo) =>
        client
        ->Promise.fromOption(~error=ClientNotConnected)
        ->Promise.flatMapOk(client => client->ReBeacon.WalletClient.removePeer(peer)),
      ~kind=Logs.Beacon,
    )

  let useDeleteAll = (~client) =>
    ApiRequest.useSetter(
      ~set=(~config as _, ()) =>
        client
        ->Promise.fromOption(~error=ClientNotConnected)
        ->Promise.flatMapOk(ReBeacon.WalletClient.removeAllPeers),
      ~kind=Logs.Beacon,
    )
}

/* PERMISSIONS */

module Permissions = {
  let useLoad = (client, requestState) => {
    let get = (~config as _s, ()) =>
      client
      ->Promise.fromOption(~error=ClientNotConnected)
      ->Promise.flatMapOk(ReBeacon.WalletClient.getPermissions)

    ApiRequest.useLoader(~get, ~kind=Logs.Beacon, ~requestState, ())
  }

  let useDelete = (~client) =>
    ApiRequest.useSetter(
      ~set=(~config as _s, accountIdentifier: Beacon.accountIdentifier) =>
        client
        ->Promise.fromOption(~error=ClientNotConnected)
        ->Promise.flatMapOk(client =>
          client->ReBeacon.WalletClient.removePermission(accountIdentifier)
        ),
      ~kind=Logs.Beacon,
    )

  let useDeleteAll = (~client) =>
    ApiRequest.useSetter(
      ~set=(~config as _, ()) =>
        client
        ->Promise.fromOption(~error=ClientNotConnected)
        ->Promise.flatMapOk(ReBeacon.WalletClient.removeAllPermissions),
      ~kind=Logs.Beacon,
    )
}

/* SIGNATURE */

module Signature = {
  let useSignPayload = () => {
    let config = ConfigContext.useContent()

    (~source, ~signingIntent, ~payload) =>
      NodeAPI.Signature.signPayload(config, ~source, ~signingIntent, ~payload)
  }
}
