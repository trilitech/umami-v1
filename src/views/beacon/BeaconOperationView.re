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

module type OP = {
  let make:
    (Account.t, ReBeacon.Message.Request.operationRequest) => Protocol.batch;
};

module Make = (Op: OP) => {
  [@react.component]
  let make =
      (
        ~sourceAccount,
        ~beaconRequest: ReBeacon.Message.Request.operationRequest,
        ~closeAction,
      ) => {
    let (operationApiRequest, sendOperation) =
      StoreContext.Operations.useCreate();

    let operation =
      React.useMemo1(
        () => Op.make(sourceAccount, beaconRequest),
        [|beaconRequest|],
      );

    let loading = operationApiRequest->ApiRequest.isLoading;
    let sendOperation = (~operation, intent) =>
      sendOperation({operation, signingIntent: intent});

    let (operationSimulateRequest, sendOperationSimulate) =
      StoreContext.Operations.useSimulate();

    let updateAccount = StoreContext.SelectedAccount.useSet();

    let (client, _) = StoreContext.Beacon.useClient();

    let onAbort = _ =>
      Promise.async(() => {
        let%Await client =
          client->Promise.fromOption(
            ~error=Errors.Generic(I18n.Errors.beacon_client_not_created),
          );

        let%AwaitMap () =
          client->ReBeacon.WalletClient.respond(
            `Error({
              type_: `error,
              id: beaconRequest.id,
              errorType: `ABORTED_ERROR,
            }),
          );

        closeAction();
      });

    let onSimulateError = _ =>
      Promise.async(() => {
        let%Await client =
          client->Promise.fromOption(
            ~error=Errors.Generic(I18n.Errors.beacon_client_not_created),
          );

        let%AwaitMap () =
          client->ReBeacon.WalletClient.respond(
            `Error({
              type_: `error,
              id: beaconRequest.id,
              errorType: `UNKNOWN_ERROR,
            }),
          );
        closeAction();
      });

    let onPressCancel = _ => {
      closeAction();
      Routes.(push(Operations));
    };

    let closing =
      operationApiRequest->ApiRequest.isDoneOk
        ? Some(ModalFormView.Close(_ => closeAction())) : None;

    let state = React.useState(() => None);

    React.useEffect1(
      () => {
        sendOperationSimulate(operation)->Promise.ignore;
        None;
      },
      [|operation|],
    );

    let sendOperation = (~operation, i) => {
      let%Await result = sendOperation(~operation, i);

      let%AwaitMap () =
        switch (client) {
        | Some(client) =>
          client->ReBeacon.WalletClient.respond(
            `OperationResponse({
              type_: `operation_response,
              id: beaconRequest.id,
              transactionHash: result.hash,
            }),
          )
        | None => Promise.ok()
        };

      updateAccount(beaconRequest.sourceAddress);
    };

    let (signStep, setSign) as signOpStep =
      React.useState(() => SignOperationView.SummaryStep);

    let title =
      switch (operationApiRequest) {
      | Done(Ok(_), _) => None
      | _ => SignOperationView.makeTitle(signStep)->Some
      };

    let back =
      switch (signStep) {
      | AdvancedOptStep(_) => Some(() => setSign(_ => SummaryStep))
      | SummaryStep => None
      };

    <ModalFormView ?title ?closing back>
      {switch (operationApiRequest) {
       | Done(Ok(result), _) =>
         <SubmittedView
           hash={result.hash}
           onPressCancel
           submitText=I18n.Btn.go_operations
         />
       | _ =>
         <>
           {<View style=FormStyles.header>
              <Typography.Overline2
                colorStyle=`highEmphasis
                fontWeightStyle=`bold
                style=styles##dapp>
                beaconRequest.appMetadata.name->React.string
              </Typography.Overline2>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##dapp>
                I18n.Expl.beacon_operation->React.string
              </Typography.Overline3>
            </View>
            ->ReactUtils.onlyWhen(signStep == SummaryStep)}
           {switch (operationSimulateRequest) {
            | ApiRequest.NotAsked
            | Loading(_) => <LoadingView style=styles##loading />
            | Done(Error(error), _) =>
              <>
                <ErrorView error />
                <View style=styles##formActionSpaceBetween>
                  <Buttons.SubmitSecondary
                    text=I18n.Btn.close
                    onPress=onSimulateError
                  />
                </View>
              </>
            | Done(Ok(dryRun), _) =>
              let secondaryButton =
                <Buttons.SubmitSecondary
                  text=I18n.Btn.reject
                  onPress=onAbort
                />;
              <SignOperationView
                source=sourceAccount
                dryRun
                signOpStep
                state
                operation
                loading
                secondaryButton
                sendOperation
              />;
            }}
         </>
       }}
    </ModalFormView>;
  };
};

module Delegate =
  Make({
    let make =
        (account, beaconRequest: ReBeacon.Message.Request.operationRequest) => {
      let delegate =
        beaconRequest.operationDetails
        ->Array.get(0)
        ->Option.map(ReBeacon.Message.Request.PartialOperation.classify)
        ->Option.flatMap(operationDetail =>
            switch (operationDetail) {
            | Delegation(delegation) => delegation.delegate
            | _ => None
            }
          );
      let delegate =
        switch (delegate) {
        | Some(d) => Protocol.Delegation.Delegate(d)
        | None => Protocol.Delegation.Undelegate(None)
        };

      ProtocolHelper.Delegation.makeSingleton(
        ~source=account,
        ~infos={delegate, options: ProtocolOptions.make()},
        (),
      );
    };
  });

module Originate =
  Make({
    let make =
        (account, beaconRequest: ReBeacon.Message.Request.operationRequest) => {
      let origination =
        beaconRequest.operationDetails
        ->Array.get(0)
        ->Option.map(ReBeacon.Message.Request.PartialOperation.classify)
        ->Option.flatMap(operationDetail =>
            switch (operationDetail) {
            | Origination(partialOrigination) => Some(partialOrigination)
            | _ => None
            }
          )
        ->Option.map(
            (
              partialOrigination: ReBeacon.Message.Request.PartialOperation.origination,
            ) => {
            ProtocolHelper.Origination.make(
              ~source=account,
              ~balance=Tez.fromMutezString(partialOrigination.balance),
              ~code=partialOrigination.script.code,
              ~storage=partialOrigination.script.storage,
              ~delegate=partialOrigination.delegate,
              (),
            )
          })
        ->Option.getUnsafe; // Not the cleanest but we should be confident we're dealing with an origination request
      origination;
    };
  });

module Transfer =
  Make({
    let make =
        (account, beaconRequest: ReBeacon.Message.Request.operationRequest) => {
      let partialTransactions =
        beaconRequest.operationDetails
        ->Array.map(ReBeacon.Message.Request.PartialOperation.classify)
        ->Array.keepMap(partialOperation =>
            switch (partialOperation) {
            | TransactionOperation(transaction) => Some(transaction)
            | _ => None
            }
          );

      let transfers =
        partialTransactions->Array.map(({destination, amount, parameters}) =>
          ProtocolHelper.Transfer.makeSimple(
            ~data={destination, amount: Tez(Tez.fromMutezString(amount))},
            ~parameter=?parameters->Option.map(a => a.value),
            ~entrypoint=?parameters->Option.map(a => a.entrypoint),
            (),
          )
        );

      ProtocolHelper.Transfer.makeBatch(~source=account, ~transfers, ());
    };
  });
