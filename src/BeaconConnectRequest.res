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

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~marginVertical=10.->dp, ~maxHeight=400.->dp, ()),
  })
}

let getName = x =>
  switch x {
  | #Custom(_) => None
  | ch => ch->Network.getDisplayedName->Js.String.toLowerCase->Some
  }

let checkOperationRequestTargetNetwork = (config: ConfigContext.env, chain: Beacon.network) =>
  chain.type_ == (config.network.chain->Network.getChainId :> string) ||
    Some(chain.type_) == config.network.chain->getName

let checkOnlyTransaction = (request: Beacon.Message.Request.operationRequest) =>
  request.operationDetails->Array.every(operationDetail => operationDetail.kind == #transaction)

let checkOnlyOneDelegation = ({operationDetails}: Beacon.Message.Request.operationRequest) =>
  operationDetails->Array.size == 1 &&
    operationDetails->Array.every(operationDetail => operationDetail.kind == #delegation)

let checkOnlyOneOrigination = ({operationDetails}: Beacon.Message.Request.operationRequest) =>
  operationDetails->Array.size == 1 &&
    operationDetails->Array.every(operationDetail => operationDetail.kind == #origination)

let useBeaconRequestModalAction = () => {
  let (request, setRequest) = React.useState(_ => None)
  let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()

  let openModal = request => {
    setRequest(_ => Some(request))
    openAction()
  }

  (request, visibleModal, openModal, closeAction)
}

let respondWithError = (client, id, errorType) =>
  Promise.async(() =>
    client
    ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
    ->Promise.flatMapOk(client =>
      client->ReBeacon.WalletClient.respond(#Error({type_: #error, id: id, errorType: errorType}))
    )
  )

module ErrorView = {
  @react.component
  let make = (~err, ~closeModal) =>
    <ModalTemplate.Dialog>
      <Typography.Headline style=FormStyles.header>
        {I18n.Title.beacon_error->React.string}
      </Typography.Headline>
      <ScrollView style={styles["container"]} alwaysBounceVertical=false>
        <Typography.Body1 colorStyle=#error style=FormStyles.textAlignCenter>
          {err->Errors.toString->React.string}
        </Typography.Body1>
      </ScrollView>
      <View style=FormStyles.formAction>
        <Buttons.Form onPress={_ => closeModal()} text=I18n.Btn.ok />
      </View>
    </ModalTemplate.Dialog>
}

type request =
  | Op(Beacon.Message.Request.operationRequest, array<Protocol.manager>)
  | Other(Beacon.Message.Request.t)

@react.component
let make = (~account) => {
  let settings = ConfigContext.useContent()
  let settingsRef = React.useRef(settings)

  settingsRef.current = settings

  let (request, visibleModal, openModal, closeModal) = useBeaconRequestModalAction()
  let (isReady, ready) = React.useState(() => true)
  open Beacon.Message.Request

  let requestData = React.useMemo1(() =>
    switch request {
    | Some(Ok(OperationRequest(request))) =>
      BeaconApiRequest.requestToBatch(request)->Result.map(batch => Op(request, batch))->Some
    | Some(Ok(request)) => Some(Ok(Other(request)))
    | Some(Error(e)) => Some(Error(e))
    | _ => None
    }
  , [request])

  let (client, _) = StoreContext.Beacon.useClient()
  let (nextRequest, doneResponding) = StoreContext.Beacon.useNextRequestState()

  let closeModal = () => {
    ready(_ => false)
    closeModal()
    doneResponding()
  }

  let setError = (client, id, errorType, msg) => {
    respondWithError(client, id, errorType)
    openModal(Error(msg))
  }

  React.useEffect3(() => {
    if isReady {
      let r = nextRequest()
      switch r {
      | Some(request) =>
        let targetSettedNetwork =
          request
          ->Beacon.Message.Request.getNetwork
          ->Option.mapWithDefault(true, network =>
            settingsRef.current->checkOperationRequestTargetNetwork(network)
          )

        if targetSettedNetwork {
          switch request {
          | PermissionRequest(_) => openModal(Ok(request))
          | SignPayloadRequest(_) => openModal(Ok(request))
          | OperationRequest(r) =>
            if (
              r->checkOnlyTransaction || (r->checkOnlyOneDelegation || r->checkOnlyOneOrigination)
            ) {
              openModal(request->Ok)
            } else {
              setError(
                client,
                r.id,
                #TRANSACTION_INVALID_ERROR,
                BeaconApiRequest.OperationNotSupported,
              )
            }
          | _ => ()
          }
        } else {
          setError(
            client,
            request->Beacon.Message.Request.getId,
            #NETWORK_NOT_SUPPORTED,
            BeaconApiRequest.NetworkMismatch,
          )
        }
      | None => ()
      }
    }
    None
  }, (isReady, nextRequest, openModal))

  <>
    <ModalAction visible=visibleModal onRequestClose={() => ready(_ => true)}>
      {requestData->ReactUtils.mapOpt(x =>
        switch x {
        | Ok(Other(PermissionRequest(r))) =>
          <BeaconPermissionView account permissionRequest=r closeAction=closeModal />
        | Ok(Op(r, operation))
          if r->checkOnlyTransaction || (r->checkOnlyOneOrigination || r->checkOnlyOneDelegation) =>
          <BeaconOperationView
            beaconRequest=r operation sourceAccount=account closeAction=closeModal
          />
        | Ok(Other(SignPayloadRequest(r))) =>
          <BeaconSignPayloadView
            sourceAccount=account signPayloadRequest=r closeAction=closeModal
          />
        | Ok(Op(_, _))
        | Ok(Other(OperationRequest(_)))
        | Ok(Other(BroadcastRequest(_))) =>
          <ErrorView err=BeaconApiRequest.BeaconNotHandled closeModal />
        | Error(err) => <ErrorView err closeModal />
        }
      )}
    </ModalAction>
  </>
}
