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
      ~operationRequest as
        operationBeaconRequest: ReBeacon.Message.Request.operationRequest,
      ~closeAction,
    ) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let transfer =
    React.useMemo1(
      () => {
        let partialTransactions =
          operationBeaconRequest.operationDetails
          ->Array.map(ReBeacon.Message.Request.PartialOperation.classify)
          ->Array.keepMap(partialOperation =>
              switch (partialOperation) {
              | TransactionOperation(transaction) => Some(transaction)
              | _ => None
              }
            );
        {
          Transfer.source: operationBeaconRequest.sourceAddress,
          transfers:
            partialTransactions
            ->Array.map(partialTransaction =>
                {
                  Transfer.destination: partialTransaction.destination,
                  amount:
                    Tez(Tez.fromMutezString(partialTransaction.amount)),
                  tx_options: {
                    fee: None,
                    gasLimit: None,
                    storageLimit: None,
                    parameter:
                      partialTransaction.parameters->Option.map(a => a.value),
                    entrypoint:
                      partialTransaction.parameters
                      ->Option.map(a => a.entrypoint),
                  },
                }
              )
            ->List.fromArray,
          common_options: {
            fee: None,
            burnCap: None,
            forceLowFee: None,
          },
        };
      },
      [|operationBeaconRequest|],
    );

  React.useEffect1(
    () => {
      sendOperationSimulate(Operation.Simulation.transaction(transfer, None))
      ->ignore;
      None;
    },
    [|transfer|],
  );

  let updateAccount = StoreContext.SelectedAccount.useSet();
  let (operationApiRequest, sendOperation) =
    StoreContext.Operations.useCreate();
  let loading = operationApiRequest->ApiRequest.isLoading;

  let sendTransfer = (~transfer, ~password) => {
    let operation = Operation.transfer(transfer);

    sendOperation({operation, password})
    ->Future.tapOk(hash => {
        BeaconApiRequest.respond(
          `OperationResponse({
            type_: `operation_response,
            id: operationBeaconRequest.id,
            transactionHash: hash,
          }),
        )
        ->ignore
      })
    ->Future.tapOk(_ => {updateAccount(transfer.source)});
  };

  let onAbort = _ =>
    BeaconApiRequest.respond(
      `Error({
        type_: `error,
        id: operationBeaconRequest.id,
        errorType: `ABORTED_ERROR,
      }),
    )
    ->Future.tapOk(_ => closeAction())
    ->ignore;

  let onSimulateError = _ =>
    BeaconApiRequest.respond(
      `Error({
        type_: `error,
        id: operationBeaconRequest.id,
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
    PasswordFormView.usePasswordForm(sendTransfer(~transfer));

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
             operationBeaconRequest.appMetadata.name->React.string
           </Typography.Overline2>
           <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
             I18n.expl#beacon_operation->React.string
           </Typography.Overline3>
         </View>
         {switch (operationSimulateRequest) {
          | NotAsked
          | Loading(_) => <LoadingView style=styles##loading />
          | Done(Error(error), _) =>
            <>
              <ErrorView error={error->API.Error.fromApiToString} />
              <View style=styles##formActionSpaceBetween>
                <Buttons.SubmitSecondary
                  text=I18n.btn#close
                  onPress=onSimulateError
                />
              </View>
            </>
          | Done(Ok(dryRun), _) =>
            <>
              <OperationSummaryView.Transactions transfer dryRun />
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
