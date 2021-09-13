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

let checkOperationRequestTargetNetwork =
    (settings: ConfigFile.t, chain: ReBeacon.network) => {
  chain.type_ == settings->ConfigUtils.chainId
  || chain.type_ == settings->ConfigUtils.chainId->Network.getName;
};

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

let useSourceAccount = request => {
  open ReBeacon.Message.Request;
  let address =
    switch (request) {
    | Some(Ok(OperationRequest(r))) => r.sourceAddress->Some
    | Some(Ok(BroadcastRequest(_)))
    | Some(Ok(PermissionRequest(_)))
    | Some(Ok(SignPayloadRequest(_)))
    | Some(Error(_))
    | None => None
    };

  StoreContext.Accounts.useGetFromOptAddress(address);
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
    | (Some(account), Some(request)) => Some((account, request))
    | _ => None
    };

  let addToast = LogsContext.useToast();
  let (error, setError) = React.useState(_ => None);
  let (errorMessage, setErrorMessage) = React.useState(_ => None);

  let (client, _) = StoreContext.Beacon.useClient();
  let (nextRequest, doneResponding) =
    StoreContext.Beacon.useNextRequestState();

  let close = () => {
    closeModal();
    setError(_ => None);
    doneResponding();
  };

  React.useEffect1(
    () => {
      switch (error) {
      | Some(error) =>
        setErrorMessage(_ => Some(error));
        addToast(Logs.log(~kind=Logs.Error, ~origin=Beacon, error));
        openModal(Error(Errors.Generic(error)));
      | None => closeModal()
      };
      None;
    },
    [|error|],
  );

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
            if (r->checkOperationRequestHasOnlyTransaction) {
              openModal(request->Ok);
            } else if (r->checkOperationRequestHasOnlyOneDelegation) {
              openModal(request->Ok);
            } else {
              FutureEx.async(() => {
                let%FRes client =
                  client->FutureEx.fromOption(
                    ~error=
                      Errors.Generic(I18n.errors#beacon_client_not_created),
                  );
                let%FResMap () =
                  client->ReBeacon.WalletClient.respond(
                    `Error({
                      type_: `error,
                      id: r.id,
                      errorType: `TRANSACTION_INVALID_ERROR,
                    }),
                  );
                setError(_ =>
                  Some(I18n.errors#beacon_transaction_not_supported)
                );
              });
            }
          | _ => ()
          };
        } else {
          FutureEx.async(() => {
            let%FRes client =
              client->FutureEx.fromOption(
                ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
              );
            let%FResMap () =
              client->ReBeacon.WalletClient.respond(
                `Error({
                  type_: `error,
                  id: request->ReBeacon.Message.Request.getId,
                  errorType: `NETWORK_NOT_SUPPORTED,
                }),
              );
            setError(_ => Some(I18n.errors#beacon_request_network_missmatch));
          });
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
         | (sourceAccount, Ok(OperationRequest(r)))
             when r->checkOperationRequestHasOnlyTransaction =>
           <BeaconOperationView.Transfer
             sourceAccount
             beaconRequest=r
             closeAction=closeModal
           />
         | (sourceAccount, Ok(OperationRequest(r)))
             when r->checkOperationRequestHasOnlyOneDelegation =>
           <BeaconOperationView.Delegate
             beaconRequest=r
             sourceAccount
             closeAction=closeModal
           />
         | (_, Ok(SignPayloadRequest(r))) =>
           <BeaconSignPayloadView
             signPayloadRequest=r
             closeAction=closeModal
           />
         | (_, Ok(BroadcastRequest(_) | OperationRequest(_)))
         | (_, Error(_)) =>
           <ModalTemplate.Dialog>
             <Typography.Headline style=FormStyles.header>
               I18n.title#beacon_error->React.string
             </Typography.Headline>
             {errorMessage->ReactUtils.mapOpt(errorMessage =>
                <ScrollView style=styles##container alwaysBounceVertical=false>
                  <Typography.Body1
                    colorStyle=`error style=FormStyles.textAlignCenter>
                    errorMessage->React.string
                  </Typography.Body1>
                </ScrollView>
              )}
             <View style=FormStyles.formAction>
               <Buttons.Form
                 onPress={_ => setError(_ => None)}
                 text=I18n.btn#ok
               />
             </View>
           </ModalTemplate.Dialog>,
       )}
    </ModalAction>
  </>;
};
