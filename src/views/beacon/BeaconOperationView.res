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
    "title": style(~marginBottom=8.->dp, ~textAlign=#center, ()),
    "dapp": style(~marginBottom=4.->dp, ~textAlign=#center, ()),
    "loading": style(~minHeight=400.->dp, ()),
    "formActionSpaceBetween": StyleSheet.flatten([
      FormStyles.formActionSpaceBetween,
      style(~marginTop=12.->dp, ()),
    ]),
  })
}

module type OP = {
  let make: (Account.t, ReBeacon.Message.Request.operationRequest) => Protocol.batch
}

@react.component
let make = (
  ~sourceAccount,
  ~beaconRequest: ReBeacon.Message.Request.operationRequest,
  ~operation,
  ~closeAction,
) => {
  let (operationApiRequest, sendOperation) = StoreContext.Operations.useCreate()

  let loading = operationApiRequest->ApiRequest.isLoading
  let sendOperation = (~operation, intent) =>
    sendOperation({operation: operation, signingIntent: intent})

  let (operationSimulateRequest, sendOperationSimulate) = StoreContext.Operations.useSimulate()

  let updateAccount = StoreContext.SelectedAccount.useSet()

  let (client, _) = StoreContext.Beacon.useClient()

  let onAbort = _ =>
    Promise.async(() =>
      client
      ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
      ->Promise.flatMapOk(client =>
        client->ReBeacon.WalletClient.respond(
          #Error({
            type_: #error,
            id: beaconRequest.id,
            errorType: #ABORTED_ERROR,
          }),
        )
      )
      ->Promise.mapOk(() => closeAction())
    )

  let onSimulateError = _ =>
    Promise.async(() =>
      client
      ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
      ->Promise.flatMapOk(client =>
        client->ReBeacon.WalletClient.respond(
          #Error({
            type_: #error,
            id: beaconRequest.id,
            errorType: #UNKNOWN_ERROR,
          }),
        )
      )
      ->Promise.mapOk(() => closeAction())
    )

  let onPressCancel = _ => {
    closeAction()
    open Routes
    push(Operations)
  }

  let closing =
    operationApiRequest->ApiRequest.isDoneOk ? Some(ModalFormView.Close(_ => closeAction())) : None

  let state = React.useState(() => None)

  React.useEffect1(() => {
    sendOperationSimulate(operation)->Promise.ignore
    None
  }, [operation])

  let sendOperation = (~operation, i) =>
    sendOperation(~operation, i)
    ->Promise.flatMapOk(result =>
      switch client {
      | Some(client) =>
        client->ReBeacon.WalletClient.respond(
          #OperationResponse({
            type_: #operation_response,
            id: beaconRequest.id,
            transactionHash: result.hash,
          }),
        )
      | None => Promise.ok()
      }
    )
    ->Promise.mapOk(() => updateAccount(beaconRequest.sourceAddress))

  let (signStep, setSign) as signOpStep = React.useState(() => SignOperationView.SummaryStep)

  let title = switch operationApiRequest {
  | Done(Ok(_), _) => None
  | _ => SignOperationView.makeTitle(signStep)->Some
  }

  let back = switch signStep {
  | AdvancedOptStep(_) => Some(() => setSign(_ => SummaryStep))
  | SummaryStep => None
  }

  <ModalFormView ?title ?closing back>
    {switch operationApiRequest {
    | Done(Ok(result), _) =>
      <SubmittedView hash=result.hash onPressCancel submitText=I18n.Btn.go_operations />
    | _ => <>
        {<View style=FormStyles.header>
          <Typography.Overline2
            colorStyle=#highEmphasis fontWeightStyle=#bold style={styles["dapp"]}>
            {beaconRequest.appMetadata.name->React.string}
          </Typography.Overline2>
          <Typography.Overline3 colorStyle=#highEmphasis style={styles["dapp"]}>
            {I18n.Expl.beacon_operation->React.string}
          </Typography.Overline3>
        </View>->ReactUtils.onlyWhen(signStep == SummaryStep)}
        {switch operationSimulateRequest {
        | ApiRequest.NotAsked
        | Loading(_) =>
          <LoadingView style={styles["loading"]} />
        | Done(Error(error), _) => <>
            <ErrorView error />
            <View style={styles["formActionSpaceBetween"]}>
              <Buttons.SubmitSecondary text=I18n.Btn.close onPress=onSimulateError />
            </View>
          </>
        | Done(Ok(dryRun), _) =>
          let secondaryButton = <Buttons.SubmitSecondary text=I18n.Btn.reject onPress=onAbort />
          <SignOperationView
            source=sourceAccount
            dryRun
            signOpStep
            state
            operation
            loading
            secondaryButton
            sendOperation
          />
        }}
      </>
    }}
  </ModalFormView>
}
