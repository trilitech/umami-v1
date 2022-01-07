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

open ReactNative;
open Let;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~marginVertical=10.->dp, ~maxHeight=400.->dp, ()),
    })
  );

let getName =
  fun
  | `Custom(_) => None
  | ch => ch->Network.getDisplayedName->Js.String.toLowerCase->Some;

let checkOperationRequestTargetNetwork =
    (config: ConfigContext.env, chain: ReBeacon.network) => {
  chain.type_ == config.network.chain->Network.getChainId
  || Some(chain.type_) == config.network.chain->getName;
};

let checkOnlyTransaction =
    (request: ReBeacon.Message.Request.operationRequest) => {
  request.operationDetails
  ->Array.every(operationDetail => operationDetail.kind == `transaction);
};

let checkOnlyOneDelegation =
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

let useSourceAccount = request => {
  open ReBeacon.Message.Request;
  let address =
    switch (request) {
    | Some(Ok(SignPayloadRequest(r))) => r.sourceAddress->Some
    | Some(Ok(OperationRequest(r))) => r.sourceAddress->Some
    | Some(Ok(BroadcastRequest(_)))
    | Some(Ok(PermissionRequest(_)))
    | Some(Error(_))
    | None => None
    };

  StoreContext.Accounts.useGetFromOptAddress(address);
};

let respondWithError = (client, id, errorType) =>
  Promise.async(() => {
    let%Await client =
      client->Promise.fromOption(
        ~error=Errors.Generic(I18n.Errors.beacon_client_not_created),
      );
    let%AwaitMap () =
      client->ReBeacon.WalletClient.respond(
        `Error({type_: `error, id, errorType}),
      );
    ();
  });

module ErrorView = {
  [@react.component]
  let make = (~msg, ~closeModal) => {
    <ModalTemplate.Dialog>
      <Typography.Headline style=FormStyles.header>
        I18n.Title.beacon_error->React.string
      </Typography.Headline>
      <ScrollView style=styles##container alwaysBounceVertical=false>
        <Typography.Body1 colorStyle=`error style=FormStyles.textAlignCenter>
          msg->React.string
        </Typography.Body1>
      </ScrollView>
      <View style=FormStyles.formAction>
        <Buttons.Form onPress={_ => closeModal()} text=I18n.Btn.ok />
      </View>
    </ModalTemplate.Dialog>;
  };
};

[@react.component]
let make = () => {
  let settings = ConfigContext.useContent();
  let settingsRef = React.useRef(settings);

  settingsRef.current = settings;

  let (request, visibleModal, openModal, closeModal) =
    useBeaconRequestModalAction();

  let sourceAccount = useSourceAccount(request);
  let requestData =
    switch (sourceAccount, request) {
    | (account, Some(request)) => Some((account, request))
    | _ => None
    };

  let (client, _) = StoreContext.Beacon.useClient();
  let (nextRequest, doneResponding) =
    StoreContext.Beacon.useNextRequestState();

  let close = () => {
    closeModal();
    doneResponding();
  };

  let setError = (client, id, errorType, msg) => {
    respondWithError(client, id, errorType);
    openModal(Error(msg));
  };

  React.useEffect1(
    () => {
      switch (nextRequest()) {
      | Some(request) =>
        let targetSettedNetwork =
          request
          ->ReBeacon.Message.Request.getNetwork
          ->Option.mapWithDefault(true, network =>
              settingsRef.current->checkOperationRequestTargetNetwork(network)
            );

        if (targetSettedNetwork) {
          switch (request) {
          | PermissionRequest(_) => openModal(Ok(request))
          | SignPayloadRequest(_) => openModal(Ok(request))
          | OperationRequest(r) =>
            if (r->checkOnlyTransaction) {
              openModal(request->Ok);
            } else if (r->checkOnlyOneDelegation) {
              openModal(request->Ok);
            } else {
              setError(
                client,
                r.id,
                `TRANSACTION_INVALID_ERROR,
                I18n.Errors.beacon_transaction_not_supported,
              );
            }
          | _ => ()
          };
        } else {
          setError(
            client,
            request->ReBeacon.Message.Request.getId,
            `NETWORK_NOT_SUPPORTED,
            I18n.Errors.beacon_request_network_missmatch,
          );
        };
      | None => ()
      };
      None;
    },
    [|nextRequest|],
  );

  <>
    <ModalAction visible=visibleModal onRequestClose=close>
      {requestData->ReactUtils.mapOpt(
         fun
         | (_, Ok(PermissionRequest(r))) =>
           <BeaconPermissionView permissionRequest=r closeAction=closeModal />

         | (Some(sourceAccount), Ok(OperationRequest(r)))
             when r->checkOnlyTransaction =>
           <BeaconOperationView.Transfer
             sourceAccount
             beaconRequest=r
             closeAction=closeModal
           />
         | (Some(sourceAccount), Ok(OperationRequest(r)))
             when r->checkOnlyOneDelegation =>
           <BeaconOperationView.Delegate
             beaconRequest=r
             sourceAccount
             closeAction=closeModal
           />
         | (Some(sourceAccount), Ok(SignPayloadRequest(r))) =>
           <BeaconSignPayloadView
             sourceAccount
             signPayloadRequest=r
             closeAction=closeModal
           />

         | (None, Ok(OperationRequest(_) | SignPayloadRequest(_))) =>
           <ErrorView msg=I18n.Errors.beacon_cant_handle closeModal />
         | (_, Ok(BroadcastRequest(_) | OperationRequest(_))) =>
           <ErrorView msg=I18n.Errors.beacon_cant_handle closeModal />
         | (_, Error(msg)) => <ErrorView msg closeModal />,
       )}
    </ModalAction>
  </>;
};
