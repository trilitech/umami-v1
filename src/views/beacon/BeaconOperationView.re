open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "dapp": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "loading": style(~minHeight=400.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~operationRequest as
        operationBeaconRequest: ReBeacon.Message.Request.operationRequest,
      ~beaconRespond,
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
              | PartialTransactionOperation(partialTransaction) =>
                Some(partialTransaction)
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
                    XTZ(
                      ProtocolXTZ.fromMutezString(partialTransaction.amount),
                    ),
                  tx_options: {
                    fee: None,
                    gasLimit: None,
                    storageLimit: None,
                    parameter: None,
                    entrypoint: None,
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

  let (source, destinations) = SendView.sourceDestination(transfer);

  let updateAccount = StoreContext.SelectedAccount.useSet();
  let (operationApiRequest, sendOperation) =
    StoreContext.Operations.useCreate();
  let loading = operationApiRequest->ApiRequest.isLoading;

  let sendTransfer = (transfer, password) => {
    let operation = Operation.transfer(transfer);

    let ((sourceAddress, _), _) = SendView.sourceDestination(transfer);

    sendOperation({operation, password})
    ->Future.tapOk(hash => {
        beaconRespond(
          ReBeacon.Message.ResponseInput.OperationResponse({
            id: operationBeaconRequest.id,
            transactionHash: hash,
          }),
        )
        ->ignore
      })
    ->Future.tapOk(_ => {updateAccount(sourceAddress)});
  };

  let onAbort = _ =>
    if (operationApiRequest->ApiRequest.isDoneOk) {
      closeAction();
    } else {
      beaconRespond(
        ReBeacon.Message.ResponseInput.Error({
          id: operationBeaconRequest.id,
          errorType: `ABORTED_ERROR,
        }),
      )
      ->Future.tapOk(_ => closeAction())
      ->ignore;
    };

  let onPressCancel = _ => {
    closeAction();
    Routes.(push(Operations));
  };

  let closeButton = <Buttons.SubmitSecondary text="REJECT" onPress=onAbort />;

  <ModalTemplate.Form headerRight=closeButton>
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
             "Requests Operation"->React.string
           </Typography.Overline3>
         </View>
         {switch (operationSimulateRequest) {
          | NotAsked
          | Loading(_) => <LoadingView style=styles##loading />
          | Done(Error(error), _) =>
            <ErrorView error={error->API.Error.fromApiToString} />
          | Done(Ok(dryRun), _) =>
            <SignOperationView
              source
              destinations
              content={SendView.buildSummaryContent(transfer, dryRun)}
              showCurrency=SendView.showAmount
              sendOperation={sendTransfer(transfer)}
              loading
            />
          }}
       </>
     }}
  </ModalTemplate.Form>;
};
