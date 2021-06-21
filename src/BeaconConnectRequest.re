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

let useBeaconPermissionRequestModalAction = () => {
  let (permissionRequest, setPermissionRequest) = React.useState(_ => None);
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let openModal = permissionRequest => {
    setPermissionRequest(_ => Some(permissionRequest));
    openAction();
  };

  (permissionRequest, visibleModal, openModal, closeAction);
};

let useBeaconOperationRequestModalAction = () => {
  let (operationRequest, setOperationRequest) = React.useState(_ => None);
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

  let openModal = operationRequest => {
    setOperationRequest(_ => Some(operationRequest));
    openAction();
  };

  (operationRequest, visibleModal, openModal, closeAction);
};

[@react.component]
let make = () => {
  let (
    permissionRequest,
    visibleModalPermission,
    openPermission,
    closePermission,
  ) =
    useBeaconPermissionRequestModalAction();
  let (operationRequest, visibleModalOperation, openOperation, closeOperation) =
    useBeaconOperationRequestModalAction();
  let settings = SdkContext.useSettings();

  React.useEffect0(() => {
    {
      client
      ->ReBeacon.WalletClient.init
      ->FutureJs.fromPromise(Js.String.make)
      ->Future.flatMapOk(_ =>
          client
          ->ReBeacon.WalletClient.connect(message => {
              let request = message->ReBeacon.Message.Request.classify;
              switch (request) {
              | PermissionRequest(request) =>
                if (request.network
                    ->checkOperationRequestTargetNetwork(
                        settings->AppSettings.network,
                      )) {
                  openPermission(request);
                } else {
                  client
                  ->ReBeacon.WalletClient.respond(
                      ReBeacon.Message.ResponseInput.Error({
                        id: request.id,
                        errorType: `NETWORK_NOT_SUPPORTED,
                      })
                      ->ReBeacon.Message.ResponseInput.toObj,
                    )
                  ->FutureJs.fromPromise(Js.String.make)
                  ->Future.get(Js.log);
                }
              | OperationRequest(request) =>
                let targetSettedNetwork =
                  request.network
                  ->checkOperationRequestTargetNetwork(
                      settings->AppSettings.network,
                    );
                if (targetSettedNetwork
                    && request->checkOperationRequestHasOnlyTransaction) {
                  openOperation(request);
                } else {
                  client
                  ->ReBeacon.WalletClient.respond(
                      ReBeacon.Message.ResponseInput.Error({
                        id: request.id,
                        errorType:
                          targetSettedNetwork
                            ? `TRANSACTION_INVALID_ERROR
                            : `NETWORK_NOT_SUPPORTED,
                      })
                      ->ReBeacon.Message.ResponseInput.toObj,
                    )
                  ->FutureJs.fromPromise(Js.String.make)
                  ->Future.get(Js.log);
                };
              | _ => ()
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

  let beaconRespond = (responseInput: ReBeacon.Message.ResponseInput.t) => {
    client
    ->ReBeacon.WalletClient.respond(
        responseInput->ReBeacon.Message.ResponseInput.toObj,
      )
    ->FutureJs.fromPromise(Js.String.make);
  };

  <>
    <ModalAction visible=visibleModalPermission onRequestClose=closePermission>
      {permissionRequest->ReactUtils.mapOpt(permissionRequest => {
         <BeaconPermissionView
           permissionRequest
           beaconRespond
           closeAction=closePermission
         />
       })}
    </ModalAction>
    <ModalAction visible=visibleModalOperation onRequestClose=closeOperation>
      {operationRequest->ReactUtils.mapOpt(operationRequest => {
         <BeaconOperationView
           operationRequest
           beaconRespond
           closeAction=closeOperation
         />
       })}
    </ModalAction>
  </>;
};
