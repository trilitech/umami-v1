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

type id = string;
type version = string;
type senderId = string;
type accountIdentifier = string;
type appMetadata = {
  senderId: string,
  name: string,
};
type network = {
  [@bs.as "type"]
  type_: string,
};
type scopes = array(string);
type operationDetails;
type sourceAddress = string;
type publicKey = string;
type transactionHash = string;

type signingType = [ | `raw | `operation | `micheline];

module Message = {
  module Request = {
    module PartialOperation = {
      type partialOperationType = [
        | `transaction
        | `activate_account
        | `ballot
        | `delegation
        | `endorsement
        | `origination
        | `proposals
        | `reveal
        | `seed_nonce_revelation
      ];
      type basePartialOperation = {kind: partialOperationType};

      type transaction = {
        amount: string,
        destination: string,
        parameters: option(ProtocolOptions.TransactionParameters.t),
      };

      type activateAccount = {
        pkh: string,
        secret: string,
      };

      type ballot = {
        period: string,
        proposal: string,
        ballot: [ | `nay | `yay | `pass],
      };

      type delegation = {delegate: option(string)};

      type endorsement = {level: string};

      type origination = {
        balance: string,
        delegate: option(string),
        script: string,
      };

      type proposals = {
        period: string,
        proposals: array(string),
      };

      type reveal = {public_key: publicKey};

      type seedNonceRevelation = {
        level: string,
        nonce: string,
      };

      type t =
        | TransactionOperation(transaction)
        | ActivateAccount(activateAccount)
        | Ballot(ballot)
        | Delegation(delegation)
        | Endorsement(endorsement)
        | Origination(origination)
        | Proposals(proposals)
        | Reveal(reveal)
        | SeedNonceRevelation(seedNonceRevelation);

      let classify = (partialOperation: basePartialOperation): t => {
        switch (partialOperation) {
        | {kind: `transaction} =>
          TransactionOperation(partialOperation->Obj.magic)
        | {kind: `activate_account} =>
          ActivateAccount(partialOperation->Obj.magic)
        | {kind: `ballot} => Ballot(partialOperation->Obj.magic)
        | {kind: `delegation} => Delegation(partialOperation->Obj.magic)
        | {kind: `endorsement} => Endorsement(partialOperation->Obj.magic)
        | {kind: `origination} => Origination(partialOperation->Obj.magic)
        | {kind: `proposals} => Proposals(partialOperation->Obj.magic)
        | {kind: `reveal} => Reveal(partialOperation->Obj.magic)
        | {kind: `seed_nonce_revelation} =>
          SeedNonceRevelation(partialOperation->Obj.magic)
        };
      };
    };

    type messageType = [
      | `permission_request
      | `operation_request
      | `sign_payload_request
      | `broadcast_request
    ];

    type baseMessage = {
      [@bs.as "type"]
      type_: messageType,
    };

    type permissionRequest = {
      id,
      version,
      senderId,
      appMetadata,
      network,
      scopes,
    };

    type operationRequest = {
      id,
      version,
      senderId,
      appMetadata,
      network,
      operationDetails: array(PartialOperation.basePartialOperation),
      sourceAddress,
    };

    type signPayloadRequest = {
      id,
      version,
      senderId,
      appMetadata,
      signingType,
      payload: string,
      sourceAddress,
    };

    type broadcastRequest = {
      id,
      version,
      senderId,
      appMetadata,
      network,
      signedTransaction: string,
    };

    type t =
      | PermissionRequest(permissionRequest)
      | OperationRequest(operationRequest)
      | SignPayloadRequest(signPayloadRequest)
      | BroadcastRequest(broadcastRequest);

    let classify = (message: baseMessage): t => {
      switch (message) {
      | {type_: `permission_request} => PermissionRequest(message->Obj.magic)
      | {type_: `operation_request} => OperationRequest(message->Obj.magic)
      | {type_: `sign_payload_request} =>
        SignPayloadRequest(message->Obj.magic)
      | {type_: `broadcast_request} => BroadcastRequest(message->Obj.magic)
      };
    };

    let getId = (request: t) =>
      switch (request) {
      | PermissionRequest({id})
      | OperationRequest({id})
      | BroadcastRequest({id})
      | SignPayloadRequest({id}) => id
      };

    let getNetwork = (request: t) =>
      switch (request) {
      | PermissionRequest({network})
      | OperationRequest({network})
      | BroadcastRequest({network}) => Some(network)
      | SignPayloadRequest(_) => None
      };
  };

  module ResponseInput = {
    type permissionResponse = {
      [@bs.as "type"]
      type_: [ | `permission_response],
      id,
      network,
      scopes,
      publicKey,
    };

    type operationResponse = {
      [@bs.as "type"]
      type_: [ | `operation_response],
      id,
      transactionHash,
    };

    type signPayloadResponse = {
      [@bs.as "type"]
      type_: [ | `sign_payload_response],
      id,
      signingType,
      signature: string,
    };

    type broadcastResponse = {
      [@bs.as "type"]
      type_: [ | `broadcast_response],
      id,
      transactionHash,
    };

    type acknowledge = {
      [@bs.as "type"]
      type_: [ | `acknowledge],
      id,
    };

    type error = {
      [@bs.as "type"]
      type_: [ | `error],
      id,
      errorType: [
        | `BROADCAST_ERROR
        | `NETWORK_NOT_SUPPORTED
        | `NO_ADDRESS_ERROR
        | `NO_PRIVATE_KEY_FOUND_ERROR
        | `NOT_GRANTED_ERROR
        | `PARAMETERS_INVALID_ERROR
        | `TOO_MANY_OPERATIONS
        | `TRANSACTION_INVALID_ERROR
        | `SIGNATURE_TYPE_NOT_SUPPORTED
        | `ABORTED_ERROR
        | `UNKNOWN_ERROR
      ],
    };
  };
};

type peerInfo = {
  id: string,
  name: string,
  publicKey,
  relayServer: string,
  senderId,
  version,
};
type permissionInfo = {
  accountIdentifier,
  address: string,
  network,
  appMetadata,
  publicKey,
  scopes,
  senderId,
};
type transportType;

module Serializer = {
  type t;

  [@bs.module "@airgap/beacon-sdk"] [@bs.new]
  external make: unit => t = "Serializer";
  [@bs.send]
  external deserialize: (t, string) => Js.Promise.t(peerInfo) = "deserialize";
};

module WalletClient = {
  type t;

  type options = {name: string};

  [@bs.module "@airgap/beacon-sdk"] [@bs.new]
  external make: options => t = "WalletClient";

  [@bs.send] external init: t => Js.Promise.t(transportType) = "init";

  [@bs.send]
  external connect:
    (t, Message.Request.baseMessage => unit) => Js.Promise.t(unit) =
    "connect";

  [@bs.send]
  external addPeer: (t, peerInfo) => Js.Promise.t(unit) = "addPeer";

  [@bs.send]
  external removePeer: (t, peerInfo) => Js.Promise.t(unit) = "removePeer";

  [@bs.send]
  external getPeers: t => Js.Promise.t(array(peerInfo)) = "getPeers";

  [@bs.send]
  external removePermission: (t, accountIdentifier) => Js.Promise.t(unit) =
    "removePermission";

  [@bs.send]
  external getPermissions: t => Js.Promise.t(array(permissionInfo)) =
    "getPermissions";

  [@bs.send]
  external respond:
    (
      t,
      [@bs.unwrap] [
        | `PermissionResponse(Message.ResponseInput.permissionResponse)
        | `OperationResponse(Message.ResponseInput.operationResponse)
        | `SignPayloadResponse(Message.ResponseInput.signPayloadResponse)
        | `BroadcastResponse(Message.ResponseInput.broadcastResponse)
        | `Acknowledge(Message.ResponseInput.acknowledge)
        | `Error(Message.ResponseInput.error)
      ]
    ) =>
    Js.Promise.t(unit) =
    "respond";

  [@bs.send] external destroy: t => Js.Promise.t(unit) = "destroy";
};