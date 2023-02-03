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

module LegacyError = {
  // These variants are duplicated against Beacon.Error.beaconErrors
  // and have to be maped one by one...
  // Maybe there is a way to avoid this with a functor?
  type Errors.t +=
    | NoMatchingRequest
    | EncodedPayloadNeedString
    | MessageNotHandled
    | CouldNotDecryptMessage
    | AppMetadataNotFound
    | ShouldNotWork
    | ContainerNotFound
    | PlatformUnknown
    | PairingRequestParsing

  let parse = e =>
    switch Beacon.Error.parse(e.RawJsError.message) {
    | NoMatchingRequest => NoMatchingRequest
    | EncodedPayloadNeedString => EncodedPayloadNeedString
    | MessageNotHandled => MessageNotHandled
    | CouldNotDecryptMessage => CouldNotDecryptMessage
    | AppMetadataNotFound => AppMetadataNotFound
    | ShouldNotWork => ShouldNotWork
    | ContainerNotFound => ContainerNotFound
    | PlatformUnknown => PlatformUnknown
    | PairingRequestParsing => PairingRequestParsing
    | Unknown(s) => Errors.Generic(Js.String.make(s))
    }

  let fromPromiseParsed = p => p->RawJsError.fromPromiseParsed(parse)
}

module Serializer = {
  include Beacon.Serializer
  let deserialize = (t, string) => t->deserializeRaw(string)->LegacyError.fromPromiseParsed
}

module WalletClient = {
  include Beacon.WalletClient

  let init = t => t->initRaw()->LegacyError.fromPromiseParsed
  let connect = (t, cb) => t->connectRaw(cb)->LegacyError.fromPromiseParsed
  let addPeer = (t, peerInfo) => t->addPeerRaw(peerInfo)->LegacyError.fromPromiseParsed
  let removePeer = (t, peerInfo) => t->removePeerRaw(peerInfo)->LegacyError.fromPromiseParsed
  let getPeers = t => t->getPeersRaw()->LegacyError.fromPromiseParsed
  let removeAllPeers = t => t->removePeersRaw->LegacyError.fromPromiseParsed
  let removePermission = (t, accountIdentifier) =>
    t->removePermissionRaw(accountIdentifier)->LegacyError.fromPromiseParsed
  let getPermissions = t => t->getPermissionsRaw()->LegacyError.fromPromiseParsed
  let removeAllPermissions = t => t->removePermissionsRaw->LegacyError.fromPromiseParsed
  let respond = (t, responseInput) => t->respondRaw(responseInput)->LegacyError.fromPromiseParsed
  let destroy = t => t->destroyRaw->LegacyError.fromPromiseParsed
}

// Not included in Beacon lib
let parsePairingRequest = (pairingRequest: string): Result.t<Beacon.peerInfo, Errors.t> =>
  switch pairingRequest->HD.BS58Check.decode->HD.toString->Beacon.parseJsonIntoPeerInfo {
  | exception Js.Exn.Error(obj) =>
    switch Js.Exn.message(obj) {
    | Some(_) => Error(LegacyError.PairingRequestParsing)
    | None => Error(LegacyError.PairingRequestParsing)
    }
  | peerInfo => Ok(peerInfo)
  }
