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

let handleOperationRequest =
    (request: ReBeacon.Message.Request.operationRequest) => {
  //let _ = request.operationDetails->Array.map(partialOperation =>
  //4
  //)
  let partialOperation =
    request.operationDetails[0]
    ->Option.map(ReBeacon.Message.Request.PartialOperation.classify);
  switch (partialOperation) {
  | Some(PartialTransactionOperation(partialTransactionOperation)) =>
    Js.log(partialTransactionOperation)
  | _ => Js.log("unknown")
  };
  Future.value(
    ReBeacon.Message.ResponseInput.OperationResponse({
      id: "",
      transactionHash: "",
    }),
  );
};

[@react.component]
let make = () => {
  let (permissionRequest, setPermissionRequest) = React.useState(_ => None);
  let (visibleModal, openAction, closeAction) =
    ModalAction.useModalActionState();

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
                setPermissionRequest(_ => Some(request));
                openAction();
              | OperationRequest(request) =>
                handleOperationRequest(request)
                ->Future.flatMap(response =>
                    client
                    ->ReBeacon.WalletClient.respond(
                        response->ReBeacon.Message.ResponseInput.toObj,
                      )
                    ->FutureJs.fromPromise(Js.String.make)
                  )
                ->Future.get(Js.log)
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
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      {permissionRequest->ReactUtils.mapOpt(permissionRequest => {
         <BeaconAccountView permissionRequest beaconRespond closeAction />
       })}
    </ModalAction>
  </>;
};
