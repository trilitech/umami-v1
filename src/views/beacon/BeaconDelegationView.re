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

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "dapp": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "loading": style(~minHeight=400.->dp, ()),
      "formActionSpaceBetween":
        StyleSheet.flatten([|
          FormStyles.formActionSpaceBetween,
          style(~marginTop=12.->dp, ()),
        |]),
    })
  );

[@react.component]
let make =
    (
      ~delegationRequest as
        delegationBeaconRequest: ReBeacon.Message.Request.operationRequest,
      ~closeAction,
    ) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let delegation =
    React.useMemo1(
      () => {
        Protocol.makeDelegate(
          ~source=delegationBeaconRequest.sourceAddress,
          ~delegate=
            delegationBeaconRequest.operationDetails
            ->Array.get(0)
            ->Option.map(ReBeacon.Message.Request.PartialOperation.classify)
            ->Option.flatMap(operationDetail =>
                switch (operationDetail) {
                | Delegation(delegation) => delegation.delegate
                | _ => None
                }
              ),
          (),
        )
      },
      [|delegationBeaconRequest|],
    );

  React.useEffect1(
    () => {
      sendOperationSimulate(delegation->Operation.Simulation.delegation)
      ->ignore;
      None;
    },
    [|delegation|],
  );

  let updateAccount = StoreContext.SelectedAccount.useSet();
  let (operationApiRequest, sendOperation) =
    StoreContext.Operations.useCreate();
  let loading = operationApiRequest->ApiRequest.isLoading;

  let sendDelegation = (~delegation, ~password) => {
    sendOperation(OperationApiRequest.delegate(delegation, password))
    ->Future.tapOk(hash => {
        BeaconApiRequest.respond(
          `OperationResponse({
            type_: `operation_response,
            id: delegationBeaconRequest.id,
            transactionHash: hash,
          }),
        )
        ->ignore
      })
    ->Future.tapOk(_ => {
        updateAccount(delegationBeaconRequest.sourceAddress)
      });
  };

  let onAbort = _ =>
    BeaconApiRequest.respond(
      `Error({
        type_: `error,
        id: delegationBeaconRequest.id,
        errorType: `ABORTED_ERROR,
      }),
    )
    ->Future.tapOk(_ => closeAction())
    ->ignore;

  let onSimulateError = _ =>
    BeaconApiRequest.respond(
      `Error({
        type_: `error,
        id: delegationBeaconRequest.id,
        errorType: `UNKNOWN_ERROR,
      }),
    )
    ->Future.tapOk(_ => closeAction())
    ->ignore;

  let onPressCancel = _ => {
    closeAction();
    Routes.(push(Operations));
  };

  let closeButton =
    operationApiRequest->ApiRequest.isDoneOk
      ? Some(
          <ModalTemplate.HeaderButtons.Close onPress={_ => closeAction()} />,
        )
      : None;

  let (form, formFieldsAreValids) =
    PasswordFormView.usePasswordForm(sendDelegation(~delegation));

  <ModalTemplate.Form headerRight=?closeButton>
    {switch (operationApiRequest) {
     | Done(Ok(hash), _) =>
       <SubmittedView hash onPressCancel submitText=I18n.btn#go_operations />
     | _ =>
       <>
         <View style=FormStyles.header>
           <Typography.Headline style=styles##title>
             I18n.title#confirmation->React.string
           </Typography.Headline>
           <Typography.Overline2
             colorStyle=`highEmphasis fontWeightStyle=`bold style=styles##dapp>
             delegationBeaconRequest.appMetadata.name->React.string
           </Typography.Overline2>
           <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
             I18n.expl#beacon_delegation->React.string
           </Typography.Overline3>
         </View>
         {switch (operationSimulateRequest) {
          | NotAsked
          | Loading(_) => <LoadingView style=styles##loading />
          | Done(Error(error), _) =>
            <>
              <ErrorView error={error->ErrorHandler.toString} />
              <View style=styles##formActionSpaceBetween>
                <Buttons.SubmitSecondary
                  text=I18n.btn#close
                  onPress=onSimulateError
                />
              </View>
            </>
          | Done(Ok(dryRun), _) =>
            <>
              <OperationSummaryView.Delegate delegation dryRun />
              <PasswordFormView.PasswordField form />
              <View style=styles##formActionSpaceBetween>
                <Buttons.SubmitSecondary
                  text=I18n.btn#reject
                  onPress=onAbort
                />
                <Buttons.SubmitPrimary
                  text=I18n.btn#confirm
                  onPress={_event => {form.submit()}}
                  loading
                  disabledLook={!formFieldsAreValids}
                />
              </View>
            </>
          }}
       </>
     }}
  </ModalTemplate.Form>;
};
