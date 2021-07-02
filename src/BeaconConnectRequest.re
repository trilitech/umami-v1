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

let client = BeaconApiRequest.client;

module IPC = {
  type t;
  type event;
  [@bs.module "electron"] external renderer: t = "ipcRenderer";
  [@bs.send] external on: (t, string, (event, string) => unit) => unit = "on";
};

let dataFromURL = url => {
  url->Js.String2.split("data=")[1];
};

let checkOperationRequestTargetNetwork =
    (a: ReBeacon.network, b: ConfigFile.network) =>
  a.type_
  == (
       switch (b) {
       | `Mainnet => "mainnet"
       | `Testnet(c) =>
         c == Network.edo2netChain
           ? "edonet" : c == Network.florencenetChain ? "florencenet" : ""
       }
     );

let checkOperationRequestHasOnlyTransaction =
    (request: ReBeacon.Message.Request.operationRequest) => {
  request.operationDetails
  ->Array.every(operationDetail => operationDetail.kind == `transaction);
};

let checkOperationRequestHasOnlyOneDelegation =
    ({operationDetails}: ReBeacon.Message.Request.operationRequest) => {
  operationDetails->Array.size == 1
  && operationDetails->Array.every(operationDetail =>
       operationDetail.kind == `delegation
     );
};

let useBeaconRequestModalAction = () => {
  let (request, setRequest) = React.useState(_ => None);
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let openModal = request => {
    setRequest(_ => Some(request));
    openAction();
  };

  (request, visibleModal, openModal, closeAction);
};

[@react.component]
let make = () => {
  let settings = SdkContext.useSettings();

  let (
    permissionRequest,
    visibleModalPermission,
    openPermission,
    closePermission,
  ) =
    useBeaconRequestModalAction();
  let (operationRequest, visibleModalOperation, openOperation, closeOperation) =
    useBeaconRequestModalAction();
  let (
    delegationRequest,
    visibleModalDelegation,
    openDelegation,
    closeDelegation,
  ) =
    useBeaconRequestModalAction();
  let (
    signPayloadRequest,
    visibleModalSignPayload,
    openSignPayload,
    closeSignPayload,
  ) =
    useBeaconRequestModalAction();

  React.useEffect0(() => {
    {
      client
      ->ReBeacon.WalletClient.init
      ->FutureJs.fromPromise(Js.String.make)
      ->Future.flatMapOk(_ =>
          client
          ->ReBeacon.WalletClient.connect(message => {
              let request = message->ReBeacon.Message.Request.classify;

              let targetSettedNetwork =
                request
                ->ReBeacon.Message.Request.getNetwork
                ->Option.mapWithDefault(true, network =>
                    network->checkOperationRequestTargetNetwork(
                      settings->AppSettings.network,
                    )
                  );

              if (targetSettedNetwork) {
                switch (request) {
                | PermissionRequest(request) => openPermission(request)
                | SignPayloadRequest(request) => openSignPayload(request)
                | OperationRequest(request) =>
                  if (request->checkOperationRequestHasOnlyTransaction) {
                    openOperation(request);
                  } else if (request->checkOperationRequestHasOnlyOneDelegation) {
                    openDelegation(request);
                  } else {
                    client
                    ->ReBeacon.WalletClient.respond(
                        `Error({
                          type_: `error,
                          id: request.id,
                          errorType: `TRANSACTION_INVALID_ERROR,
                        }),
                      )
                    ->FutureJs.fromPromise(Js.String.make)
                    ->Future.get(Js.log);
                  }
                | _ => ()
                };
              } else {
                client
                ->ReBeacon.WalletClient.respond(
                    `Error({
                      type_: `error,
                      id: request->ReBeacon.Message.Request.getId,
                      errorType: `NETWORK_NOT_SUPPORTED,
                    }),
                  )
                ->FutureJs.fromPromise(Js.String.make)
                ->Future.get(Js.log);
              };
            })
          ->FutureJs.fromPromise(Js.String.make)
        )
      ->Future.tapOk(_ => {
          IPC.renderer->IPC.on("deeplinkURL", (_, message) => {
            ReBeacon.Serializer.(
              make()
              ->deserialize(message->dataFromURL->Option.getWithDefault(""))
            )
            ->FutureJs.fromPromise(Js.String.make)
            ->Future.flatMapOk(peer =>
                client
                ->ReBeacon.WalletClient.addPeer(peer)
                ->FutureJs.fromPromise(Js.String.make)
              )
            ->Future.get(Js.log)
          })
        })
      ->Future.get(Js.log);
    };
    None;
  });

  <>
    <ModalAction visible=visibleModalPermission onRequestClose=closePermission>
      {permissionRequest->ReactUtils.mapOpt(permissionRequest => {
         <BeaconPermissionView permissionRequest closeAction=closePermission />
       })}
    </ModalAction>
    <ModalAction visible=visibleModalOperation onRequestClose=closeOperation>
      {operationRequest->ReactUtils.mapOpt(operationRequest => {
         <BeaconOperationView operationRequest closeAction=closeOperation />
       })}
    </ModalAction>
    <ModalAction visible=visibleModalDelegation onRequestClose=closeDelegation>
      {delegationRequest->ReactUtils.mapOpt(delegationRequest => {
         <BeaconDelegationView delegationRequest closeAction=closeDelegation />
       })}
    </ModalAction>
    <ModalAction
      visible=visibleModalSignPayload onRequestClose=closeSignPayload>
      {signPayloadRequest->ReactUtils.mapOpt(signPayloadRequest => {
         <BeaconSignPayloadView
           signPayloadRequest
           closeAction=closeSignPayload
         />
       })}
    </ModalAction>
  </>;
};
