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

type id = string
type version = string
type senderId = string
type accountIdentifier = string
type appMetadata = {
  senderId: string,
  name: string,
}
type network = {
  @as("type")
  type_: string,
}
type scopes = array<string>
type operationDetails
type sourceAddress = PublicKeyHash.t
type publicKey = string
type transactionHash = string

type signingType = [#raw | #operation | #micheline]

module Message = {
  module Request = {
    module PartialOperation = {
      type partialOperationType = [
        | #transaction
        | #activate_account
        | #ballot
        | #delegation
        | #endorsement
        | #origination
        | #proposals
        | #reveal
        | #seed_nonce_revelation
      ]
      type basePartialOperation = {kind: partialOperationType}

      type transaction = {
        amount: string,
        destination: PublicKeyHash.t,
        parameters: option<ProtocolOptions.TransactionParameters.t>,
      }

      type activateAccount = {
        pkh: string,
        secret: string,
      }

      type ballot = {
        period: string,
        proposal: string,
        ballot: [#nay | #yay | #pass],
      }

      type delegation = {delegate: option<PublicKeyHash.t>}

      type endorsement = {level: string}

      type script = {
        code: ReTaquitoTypes.Code.t,
        storage: ReTaquitoTypes.Storage.t,
      }

      type origination = {
        balance: string,
        delegate: option<PublicKeyHash.t>,
        script: script,
      }

      type proposals = {
        period: string,
        proposals: array<string>,
      }

      type reveal = {public_key: publicKey}

      type seedNonceRevelation = {
        level: string,
        nonce: string,
      }

      type t =
        | Transfer(transaction)
        | ActivateAccount(activateAccount)
        | Ballot(ballot)
        | Delegation(delegation)
        | Endorsement(endorsement)
        | Origination(origination)
        | Proposals(proposals)
        | Reveal(reveal)
        | SeedNonceRevelation(seedNonceRevelation)

      external toTransfer: basePartialOperation => transaction = "%identity"
      external toActivateAccount: basePartialOperation => activateAccount = "%identity"
      external toBallot: basePartialOperation => ballot = "%identity"
      external toDelegation: basePartialOperation => delegation = "%identity"
      external toEndorsement: basePartialOperation => endorsement = "%identity"
      external toOrigination: basePartialOperation => origination = "%identity"
      external toProposals: basePartialOperation => proposals = "%identity"
      external toReveal: basePartialOperation => reveal = "%identity"
      external toSeedNonceRevelation: basePartialOperation => seedNonceRevelation = "%identity"

      let classify = (partialOperation: basePartialOperation): t =>
        switch partialOperation {
        | {kind: #transaction} => Transfer(partialOperation->toTransfer)
        | {kind: #activate_account} => ActivateAccount(partialOperation->toActivateAccount)
        | {kind: #ballot} => Ballot(partialOperation->toBallot)
        | {kind: #delegation} => Delegation(partialOperation->toDelegation)
        | {kind: #endorsement} => Endorsement(partialOperation->toEndorsement)
        | {kind: #origination} => Origination(partialOperation->toOrigination)
        | {kind: #proposals} => Proposals(partialOperation->toProposals)
        | {kind: #reveal} => Reveal(partialOperation->toReveal)
        | {kind: #seed_nonce_revelation} =>
          SeedNonceRevelation(partialOperation->toSeedNonceRevelation)
        }
    }

    type messageType = [
      | #permission_request
      | #operation_request
      | #sign_payload_request
      | #broadcast_request
    ]

    type baseMessage = {
      @as("type")
      type_: messageType,
    }

    type permissionRequest = {
      id: id,
      version: version,
      senderId: senderId,
      appMetadata: appMetadata,
      network: network,
      scopes: scopes,
    }

    type operationRequest = {
      id: id,
      version: version,
      senderId: senderId,
      appMetadata: appMetadata,
      network: network,
      operationDetails: array<PartialOperation.basePartialOperation>,
      sourceAddress: sourceAddress,
    }

    type signPayloadRequest = {
      id: id,
      version: version,
      senderId: senderId,
      appMetadata: appMetadata,
      signingType: signingType,
      payload: string,
      sourceAddress: sourceAddress,
    }

    type broadcastRequest = {
      id: id,
      version: version,
      senderId: senderId,
      appMetadata: appMetadata,
      network: network,
      signedTransaction: string,
    }

    type t =
      | PermissionRequest(permissionRequest)
      | OperationRequest(operationRequest)
      | SignPayloadRequest(signPayloadRequest)
      | BroadcastRequest(broadcastRequest)

    let classify = (message: baseMessage): t =>
      switch message {
      | {type_: #permission_request} => PermissionRequest(message->Obj.magic)
      | {type_: #operation_request} => OperationRequest(message->Obj.magic)
      | {type_: #sign_payload_request} => SignPayloadRequest(message->Obj.magic)
      | {type_: #broadcast_request} => BroadcastRequest(message->Obj.magic)
      }

    let getId = (request: t) =>
      switch request {
      | PermissionRequest({id})
      | OperationRequest({id})
      | BroadcastRequest({id})
      | SignPayloadRequest({id}) => id
      }

    let getNetwork = (request: t) =>
      switch request {
      | PermissionRequest({network})
      | OperationRequest({network})
      | BroadcastRequest({network}) =>
        Some(network)
      | SignPayloadRequest(_) => None
      }
  }

  module ResponseInput = {
    type permissionResponse = {
      @as("type")
      type_: [#permission_response],
      id: id,
      network: network,
      scopes: scopes,
      publicKey: publicKey,
    }

    type operationResponse = {
      @as("type")
      type_: [#operation_response],
      id: id,
      transactionHash: transactionHash,
    }

    type signPayloadResponse = {
      @as("type")
      type_: [#sign_payload_response],
      id: id,
      signingType: signingType,
      signature: string,
    }

    type broadcastResponse = {
      @as("type")
      type_: [#broadcast_response],
      id: id,
      transactionHash: transactionHash,
    }

    type acknowledge = {
      @as("type")
      type_: [#acknowledge],
      id: id,
    }

    type error = {
      @as("type")
      type_: [#error],
      id: id,
      errorType: [
        | #BROADCAST_ERROR
        | #NETWORK_NOT_SUPPORTED
        | #NO_ADDRESS_ERROR
        | #NO_PRIVATE_KEY_FOUND_ERROR
        | #NOT_GRANTED_ERROR
        | #PARAMETERS_INVALID_ERROR
        | #TOO_MANY_OPERATIONS
        | #TRANSACTION_INVALID_ERROR
        | #SIGNATURE_TYPE_NOT_SUPPORTED
        | #ABORTED_ERROR
        | #UNKNOWN_ERROR
      ],
    }
  }
}

type peerInfo = {
  id: string,
  name: string,
  publicKey: publicKey,
  relayServer: string,
  senderId: senderId,
  version: version,
}
type permissionInfo = {
  accountIdentifier: accountIdentifier,
  address: PublicKeyHash.t,
  network: network,
  appMetadata: appMetadata,
  publicKey: publicKey,
  scopes: scopes,
  senderId: senderId,
}
type transportType

module Error = {
  let noMatchingRequest = "No matching request found!"
  let encodedPayloadNeedString = "Encoded payload needs to be a string"
  let messageNotHandled = "Message not handled'"
  let couldNotDecryptMessage = "Could not decrypt message"
  let appMetadataNotFound = "AppMetadata not found"
  let shouldNotWork = "Should not work!"
  let containerNotFound = "container not found"
  let platformUnknown = "platform unknown"

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
    switch e.RawJsError.message {
    | s if s->Js.String2.includes(noMatchingRequest) => NoMatchingRequest
    | s if s->Js.String2.includes(encodedPayloadNeedString) => EncodedPayloadNeedString
    | s if s->Js.String2.includes(messageNotHandled) => MessageNotHandled
    | s if s->Js.String2.includes(couldNotDecryptMessage) => CouldNotDecryptMessage
    | s if s->Js.String2.includes(appMetadataNotFound) => AppMetadataNotFound
    | s if s->Js.String2.includes(shouldNotWork) => ShouldNotWork
    | s if s->Js.String2.includes(containerNotFound) => ContainerNotFound
    | s if s->Js.String2.includes(platformUnknown) => PlatformUnknown
    | s => Errors.Generic(Js.String.make(s))
    }

  let fromPromiseParsed = p => p->RawJsError.fromPromiseParsed(parse)
}

module Serializer = {
  type t

  @module("@airgap/beacon-sdk") @new
  external make: unit => t = "Serializer"

  @send
  external deserializeRaw: (t, string) => Js.Promise.t<peerInfo> = "deserialize"

  let deserialize = (t, string) => t->deserializeRaw(string)->Error.fromPromiseParsed
}

module WalletClient = {
  type t

  type options = {name: string}

  type transportStatus = [#NOT_CONNECTED | #CONNECTING | #CONNECTED]

  @module("@airgap/beacon-sdk") @new
  external make: options => t = "WalletClient"

  @send external initRaw: t => Js.Promise.t<transportType> = "init"

  let init = t => t->initRaw->Error.fromPromiseParsed

  @send
  external connectionStatus: t => transportStatus = "connectionStatus"

  @send
  external connectRaw: (t, Message.Request.baseMessage => unit) => Js.Promise.t<unit> = "connect"

  let connect = (t, cb) => t->connectRaw(cb)->Error.fromPromiseParsed

  @send
  external addPeerRaw: (t, peerInfo) => Js.Promise.t<unit> = "addPeer"

  let addPeer = (t, peerInfo) => t->addPeerRaw(peerInfo)->Error.fromPromiseParsed

  @send
  external removePeerRaw: (t, peerInfo) => Js.Promise.t<unit> = "removePeer"

  let removePeer = (t, peerInfo) => t->removePeerRaw(peerInfo)->Error.fromPromiseParsed

  @send
  external getPeersRaw: t => Js.Promise.t<array<peerInfo>> = "getPeers"

  let getPeers = t => t->getPeersRaw->Error.fromPromiseParsed

  @send
  external removePeersRaw: t => Js.Promise.t<unit> = "removeAllPeers"

  let removeAllPeers = t => t->removePeersRaw->Error.fromPromiseParsed

  @send
  external removePermissionRaw: (t, accountIdentifier) => Js.Promise.t<unit> = "removePermission"

  let removePermission = (t, accountIdentifier) =>
    t->removePermissionRaw(accountIdentifier)->Error.fromPromiseParsed

  @send
  external getPermissionsRaw: t => Js.Promise.t<array<permissionInfo>> = "getPermissions"

  let getPermissions = t => t->getPermissionsRaw->Error.fromPromiseParsed

  @send
  external removePermissionsRaw: t => Js.Promise.t<unit> = "removeAllPermissions"

  let removeAllPermissions = t => t->removePermissionsRaw->Error.fromPromiseParsed

  @send
  external respondRaw: (
    t,
    @unwrap
    [
      | #PermissionResponse(Message.ResponseInput.permissionResponse)
      | #OperationResponse(Message.ResponseInput.operationResponse)
      | #SignPayloadResponse(Message.ResponseInput.signPayloadResponse)
      | #BroadcastResponse(Message.ResponseInput.broadcastResponse)
      | #Acknowledge(Message.ResponseInput.acknowledge)
      | #Error(Message.ResponseInput.error)
    ],
  ) => Js.Promise.t<unit> = "respond"

  let respond = (t, responseInput) => t->respondRaw(responseInput)->Error.fromPromiseParsed

  @send external destroyRaw: t => Js.Promise.t<unit> = "destroy"

  let destroy = t => t->destroyRaw->Error.fromPromiseParsed
}

@scope("JSON") @val
external parseJsonIntoPeerInfo: string => peerInfo = "parse"

let parsePairingRequest = (pairingRequest: string): Result.t<peerInfo, Errors.t> =>
  switch pairingRequest->HD.BS58Check.decode->HD.toString->parseJsonIntoPeerInfo {
  | exception Js.Exn.Error(obj) =>
    switch Js.Exn.message(obj) {
    | Some(_) => Error(Error.PairingRequestParsing)
    | None => Error(Error.PairingRequestParsing)
    }
  | peerInfo => Ok(peerInfo)
  }
