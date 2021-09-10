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
  type t;
  let make: (Account.t, ReBeacon.Message.Request.operationRequest) => t;
  let makeOperation: t => Operation.t;
  let makeSimulated: t => Operation.Simulation.t;
  let makeSummary: (Protocol.simulationResults, t) => React.element;
};

module Make = (Op: OP) => {
  [@react.component]
  let make =
      (
        ~beaconRequest: ReBeacon.Message.Request.operationRequest,
        ~closeAction,
      ) => {
    let (operationApiRequest, sendOperation) =
      StoreContext.Operations.useCreate();

    let sourceAccount =
      StoreContext.Accounts.useGetFromAddress(beaconRequest.sourceAddress);

    let operation =
      React.useMemo1(
        // temporary Option.getExn utile I find a better way
        () => Op.make(sourceAccount->Option.getExn, beaconRequest),
        [|beaconRequest|],
      );

    let loading = operationApiRequest->ApiRequest.isLoading;
    let sendOperation = intent =>
      sendOperation({
        operation: operation->Op.makeOperation,
        signingIntent: intent,
      });

    let (operationSimulateRequest, sendOperationSimulate) =
      StoreContext.Operations.useSimulate();

    let updateAccount = StoreContext.SelectedAccount.useSet();

    let (client, _) = StoreContext.Beacon.useClient();

    let onAbort = _ =>
      client
      ->FutureEx.fromOption(
          ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
        )
      ->Future.flatMapOk(client =>
          client->ReBeacon.WalletClient.respond(
            `Error({
              type_: `error,
              id: beaconRequest.id,
              errorType: `ABORTED_ERROR,
            }),
          )
        )
      ->FutureEx.getOk(_ => closeAction());

    let onSimulateError = _ =>
      client
      ->FutureEx.fromOption(
          ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
        )
      ->Future.flatMapOk(client =>
          client->ReBeacon.WalletClient.respond(
            `Error({
              type_: `error,
              id: beaconRequest.id,
              errorType: `UNKNOWN_ERROR,
            }),
          )
        )
      ->FutureEx.getOk(_ => closeAction());

    let onPressCancel = _ => {
      closeAction();
      Routes.(push(Operations));
    };

    let closing =
      operationApiRequest->ApiRequest.isDoneOk
        ? Some(ModalFormView.Close(_ => closeAction())) : None;

    let ledgerState = React.useState(() => None);

    let simulatedOperation = Op.makeSimulated(operation);

    React.useEffect1(
      () => {
        sendOperationSimulate(simulatedOperation)->FutureEx.ignore;
        None;
      },
      [|operation|],
    );

    let sendOperation = i => {
      let%FRes hash = sendOperation(i);

      let%FResMap () =
        switch (client) {
        | Some(client) =>
          client->ReBeacon.WalletClient.respond(
            `OperationResponse({
              type_: `operation_response,
              id: beaconRequest.id,
              transactionHash: hash,
            }),
          )
        | None => FutureEx.ok()
        };

      updateAccount(beaconRequest.sourceAddress);
    };

    <ModalFormView title=I18n.title#confirmation ?closing>
      {switch (operationApiRequest) {
       | Done(Ok(hash), _) =>
         <SubmittedView hash onPressCancel submitText=I18n.btn#go_operations />
       | _ =>
         <>
           <View style=FormStyles.header>
             <Typography.Overline2
               colorStyle=`highEmphasis
               fontWeightStyle=`bold
               style=styles##dapp>
               beaconRequest.appMetadata.name->React.string
             </Typography.Overline2>
             <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
               I18n.expl#beacon_operation->React.string
             </Typography.Overline3>
           </View>
           {switch (operationSimulateRequest) {
            | ApiRequest.NotAsked
            | Loading(_) => <LoadingView style=styles##loading />
            | Done(Error(error), _) =>
              <>
                <ErrorView error />
                <View style=styles##formActionSpaceBetween>
                  <Buttons.SubmitSecondary
                    text=I18n.btn#close
                    onPress=onSimulateError
                  />
                </View>
              </>
            | Done(Ok(dryRun), _) =>
              let secondaryButton =
                <Buttons.SubmitSecondary
                  text=I18n.btn#reject
                  onPress=onAbort
                />;
              <>
                {Op.makeSummary(dryRun, operation)}
                <SigningBlock
                  accountKind={sourceAccount->Option.map(a => a.kind)}
                  ledgerState
                  sendOperation
                  loading
                  secondaryButton
                />
              </>;
            }}
         </>
       }}
    </ModalFormView>;
  };
};

module Delegate =
  Make({
    type t = Protocol.delegation;

    let make =
        (account, beaconRequest: ReBeacon.Message.Request.operationRequest) => {
      Protocol.makeDelegate(
        ~source=account,
        ~delegate=
          beaconRequest.operationDetails
          ->Array.get(0)
          ->Option.map(ReBeacon.Message.Request.PartialOperation.classify)
          ->Option.flatMap(operationDetail =>
              switch (operationDetail) {
              | Delegation(delegation) => delegation.delegate
              | _ => None
              }
            ),
        (),
      );
    };

    let makeSimulated = o => o->Operation.Simulation.delegation;

    let makeOperation = Operation.delegation;

    let makeSummary = (dryRun, o) =>
      <OperationSummaryView.Delegate delegation=o dryRun />;
  });

module Transfer =
  Make({
    type t = Transfer.t;

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
      {
        Transfer.source: account,
        transfers:
          partialTransactions
          ->Array.map(partialTransaction =>
              {
                Transfer.destination: partialTransaction.destination,
                amount: Tez(Tez.fromMutezString(partialTransaction.amount)),
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
    };

    let makeOperation = Operation.transfer;

    let makeSimulated = o => o->Operation.Simulation.transaction(None);

    let makeSummary = (dryRun, o) =>
      <OperationSummaryView.Transactions transfer=o dryRun />;
  });
