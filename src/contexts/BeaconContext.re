let context = React.createContext(None);

module IPC = {
  type t;
  type event;
  [@bs.module "electron"] external renderer: t = "ipcRenderer";
  [@bs.send] external on: (t, string, (event, string) => unit) => unit = "on";
};

let dataFromURL = url => {
  url->Js.String2.split("data=")[1];
};

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (client, _) =
    React.useState(() => ReBeacon.WalletClient.make({name: "umami"}));

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
              | PermissionRequest(permissionRequest) =>
                setPermissionRequest(_ => Some(permissionRequest));
                openAction();
              | _ => Js.log("unknown")
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
    <Provider value={Some(client)}> children </Provider>
    <ModalAction visible=visibleModal onRequestClose=closeAction>
      {permissionRequest->ReactUtils.mapOpt(permissionRequest => {
         <BeaconAccountView permissionRequest beaconRespond closeAction />
       })}
    </ModalAction>
  </>;
};

let useBeacon = () => React.useContext(context);
