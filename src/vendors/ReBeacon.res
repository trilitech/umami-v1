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

module Error = {
  type Errors.t += BeaconError(Beacon.Error.beaconErrors)

  let () = Errors.registerHandler("BeaconError", x =>
    switch x {
    | BeaconError(e) =>
      open Beacon.Error
      `Beacon error: ${switch e {
        | NoMatchingRequest => noMatchingRequest
        | EncodedPayloadNeedString => encodedPayloadNeedString
        | MessageNotHandled => messageNotHandled
        | CouldNotDecryptMessage => couldNotDecryptMessage
        | AppMetadataNotFound => appMetadataNotFound
        | ShouldNotWork => shouldNotWork
        | ContainerNotFound => containerNotFound
        | PlatformUnknown => platformUnknown
        | PairingRequestParsing => "Pairing request parsing"
        | Unknown(s) => s
        }}`->Some
    | _ => None
    }
  )

  let fromPromiseParsed = p =>
    p->RawJsError.fromPromiseParsed(e => BeaconError(Beacon.Error.parse(e.message)))
}

module Serializer = {
  include Beacon.Serializer
  let deserialize = (t, string) => t->deserializeRaw(string)->Error.fromPromiseParsed
}

module WalletClient = {
  include Beacon.WalletClient

  let init = t => t->initRaw()->Error.fromPromiseParsed
  let connect = (t, cb) => t->connectRaw(cb)->Error.fromPromiseParsed
  let addPeer = (t, peerInfo) => t->addPeerRaw(peerInfo)->Error.fromPromiseParsed
  let removePeer = (t, peerInfo) => t->removePeerRaw(peerInfo)->Error.fromPromiseParsed
  let getPeers = t => t->getPeersRaw()->Error.fromPromiseParsed
  let removeAllPeers = t => t->removePeersRaw->Error.fromPromiseParsed
  let removePermission = (t, accountIdentifier) =>
    t->removePermissionRaw(accountIdentifier)->Error.fromPromiseParsed
  let getPermissions = t => t->getPermissionsRaw()->Error.fromPromiseParsed
  let removeAllPermissions = t => t->removePermissionsRaw->Error.fromPromiseParsed
  let respond = (t, responseInput) => t->respondRaw(responseInput)->Error.fromPromiseParsed
  let destroy = t => t->destroyRaw->Error.fromPromiseParsed
}

// Not included in Beacon lib
let parsePairingRequest = (pairingRequest: string): Result.t<Beacon.peerInfo, Errors.t> => {
  switch pairingRequest->HD.BS58Check.decode->HD.toString->Beacon.parseJsonIntoPeerInfo {
  | exception _ =>
    Error(Error.BeaconError(Unknown(`Pairing request parsing (${pairingRequest})`)))
  | peerInfo => Ok(peerInfo)
  }
}
