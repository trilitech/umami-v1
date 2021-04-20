open ReactNative;

module FormGroupAmountWithTokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~flexDirection=`row, ()),
        "amountGroup": style(~flexGrow=1., ()),
        "tokenGroup": style(~marginLeft=14.->dp, ()),
        "tokenSelector": style(~width=100.->dp, ~marginBottom=0.->dp, ()),
      })
    );
  let tokenDecoration = (~symbol, ~style) =>
    <Typography.Body1 style> symbol->React.string </Typography.Body1>;

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~handleChange,
        ~error,
        ~selectedToken,
        ~setSelectedToken,
        ~token: option(Token.t)=?,
        ~showSelector,
        ~setValue=?,
      ) => {
    let tokens = StoreContext.Tokens.useGetAll();

    let displaySelector = showSelector && tokens->Map.String.size > 0;

    let decoration =
      switch (displaySelector, token) {
      | (true, _) => None
      | (false, None) => Some(FormGroupXTZInput.xtzDecoration)
      | (false, Some(token)) => Some(tokenDecoration(~symbol=token.symbol))
      };

    <View style=styles##container>
      <FormGroupXTZInput
        style=styles##amountGroup
        label
        value
        ?setValue
        handleChange
        error
        ?decoration
        ?token
      />
      {<FormGroup style=styles##tokenGroup>
         <FormLabel
           label=I18n.label#token
           style=FormGroupTextInput.styles##label
         />
         <TokenSelector
           selectedToken
           setSelectedToken
           style=styles##tokenSelector
         />
       </FormGroup>
       ->ReactUtils.onlyWhen(displaySelector)}
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "addTransaction": style(~marginTop=10.->dp, ()),
      "advancedOptionButton":
        style(
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          ~alignItems=`center,
          ~paddingVertical=8.->dp,
          ~marginVertical=10.->dp,
          ~paddingRight=12.->dp,
          (),
        ),
      "operationSummary": style(~marginBottom=20.->dp, ()),
    })
  );

type step =
  | SendStep
  | PasswordStep(SendForm.transaction, Protocol.simulationResults)
  | EditStep(int, (SendForm.StateLenses.state, bool))
  | BatchStep
  | SubmittedStep(string);

let stepToString = step =>
  switch (step) {
  | SendStep => "sendstep"
  | PasswordStep(_, _) => "passwordstep"
  | EditStep(_, _) => "editstep"
  | BatchStep => "batchstep"
  | SubmittedStep(_) => "submittedstep"
  };

let sourceDestination = (transfer: SendForm.transaction) => {
  let recipientLbl = I18n.title#recipient_account;
  let sourceLbl = I18n.title#sender_account;
  switch (transfer) {
  | ProtocolTransaction({source, transfers: [t]}) => (
      (source, sourceLbl),
      `One((t.destination, recipientLbl)),
    )
  | ProtocolTransaction({source, transfers}) =>
    let destinations =
      transfers->List.map(t =>
        (None, (t.destination, t.amount->ProtocolXTZ.toString))
      );
    ((source, sourceLbl), `Many(destinations));
  | TokenTransfer(
      {source, transfers: [{destination}]}: Token.Transfer.t,
      _,
    ) => (
      (source, sourceLbl),
      `One((destination, recipientLbl)),
    )
  | TokenTransfer({source, transfers}: Token.Transfer.t, _) =>
    let destinations =
      transfers->List.map(t =>
        (None, (t.destination, t.amount->Token.Repr.toNatString))
      );
    ((source, sourceLbl), `Many(destinations));
  };
};

let buildSummaryContent =
    (transaction: SendForm.transaction, dryRun: Protocol.simulationResults) => {
  let fee = (
    I18n.label#fee,
    I18n.t#xtz_amount(
      ProtocolXTZ.Infix.(dryRun.fee - dryRun.revealFee)->ProtocolXTZ.toString,
    ),
  );

  let revealFee =
    dryRun.revealFee != ProtocolXTZ.zero
      ? (
          I18n.label#implicit_reveal_fee,
          I18n.t#xtz_amount(dryRun.revealFee->ProtocolXTZ.toString),
        )
        ->Some
      : None;

  switch (transaction) {
  | ProtocolTransaction({transfers}) =>
    let amount =
      transfers->List.reduce(ProtocolXTZ.zero, (acc, {amount}) =>
        ProtocolXTZ.Infix.(acc + amount)
      );
    let subtotal = (
      I18n.label#summary_subtotal,
      I18n.t#xtz_amount(amount->ProtocolXTZ.toString),
    );
    let total = ProtocolXTZ.Infix.(amount + dryRun.fee);
    let total = (
      I18n.label#summary_total,
      I18n.t#xtz_amount(total->ProtocolXTZ.toString),
    );
    [
      subtotal,
      fee,
      ...revealFee->Option.mapWithDefault([total], r => [r, total]),
    ];
  | TokenTransfer({transfers}, token) =>
    let amount =
      transfers->List.reduce(Token.Repr.zero, (acc, {amount}) =>
        Token.Repr.add(acc, amount)
      );
    let amount = I18n.t#amount(amount->Token.Repr.toNatString, token.symbol);

    let amount = (I18n.label#send_amount, amount);

    let total = dryRun.fee;
    let total = (
      I18n.label#summary_total_tez,
      I18n.t#xtz_amount(total->ProtocolXTZ.toString),
    );

    [
      amount,
      fee,
      ...revealFee->Option.mapWithDefault([total], r => [r, total]),
    ];
  };
};

let reduceAmounts = token =>
  switch (token) {
  | Some(_) => (
      amounts =>
        amounts
        ->List.reduce(0, (total, SendForm.StateLenses.{amount}) =>
            Int.fromString(amount)->Option.getWithDefault(0) + total
          )
        ->Int.toString
    )

  | None => (
      amounts => {
        ProtocolXTZ.(
          amounts
          ->List.reduce(zero, (total, {amount}) =>
              Infix.(fromString(amount)->Option.getWithDefault(zero) + total)
            )
          ->toString
        );
      }
    )
  };

module Form = {
  let defaultInit = (account: option(Account.t)) =>
    SendForm.StateLenses.{
      amount: "",
      sender: account->Option.mapWithDefault("", a => a.address),
      recipient: "",
      fee: "",
      gasLimit: "",
      storageLimit: "",
      forceLowFee: false,
      dryRun: None,
    };

  let use = (~initValues=?, initAccount, token, onSubmit) => {
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            nonEmpty(Amount)
            + nonEmpty(Sender)
            + nonEmpty(Recipient)
            + custom(
                values =>
                  token != None
                    ? FormUtils.isValidTokenAmount(values.amount)
                    : FormUtils.isValidFloat(values.amount),
                Amount,
              )
            + custom(values => FormUtils.isValidFloat(values.fee), Fee)
            + custom(
                values => FormUtils.isValidInt(values.gasLimit),
                GasLimit,
              )
            + custom(
                values => FormUtils.isValidInt(values.storageLimit),
                StorageLimit,
              ),
          )
        );
      },
      ~onSubmit=
        f => {
          onSubmit(f);
          None;
        },
      ~initialState=
        initValues->Option.getWithDefault(defaultInit(initAccount)),
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module View = {
    open SendForm;

    let onAppear = (el, _) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity #= value;
        },
        delay: 100.,
        onComplete: () => (),
      });
    };

    let onExit = (el, _, removeElement) => {
      ReactFlipToolkit.spring({
        onUpdate: value => {
          el->ReactDOMRe.domElementToObj##style##opacity
          #= Js.Math.max_float(0., 1. -. value -. 0.3);
        },
        delay: 0.,
        onComplete: removeElement,
      });
    };

    type mode =
      | Edition(int)
      | Creation(option(unit => unit), unit => unit);

    let simulatedTransaction = (mode, batch, form: SendForm.api, token) => {
      let (batch, index) =
        switch (mode) {
        | Edition(index) =>
          let batch =
            batch
            ->List.mapWithIndex((id, elt) =>
                id == index ? (form.state.values, true) : elt
              )
            ->List.reverse;
          (batch, Some(batch->List.length - (index + 1)));

        | Creation(_) =>
          let batch = [(form.state.values, true), ...batch]->List.reverse;
          let length = batch->List.length;

          (batch, Some(length - 1));
        };

      SendForm.buildTransaction(batch, token)
      |> SendForm.toSimulation(~index?);
    };

    [@react.component]
    let make =
        (
          ~batch,
          ~advancedOptionState,
          ~tokenState,
          ~token=?,
          ~mode,
          ~form,
          ~loading,
        ) => {
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;

      let (editing, onAddToBatch, onSubmitAll, batchMode) =
        switch (mode) {
        // Edition state is always on batch mode
        | Edition(_) => (true, None, None, true)
        | Creation(batch, submit) => (
            false,
            batch,
            Some(submit),
            batch == None,
          )
        };

      let submitLabel =
        editing
          ? I18n.btn#update
          : batchMode ? I18n.btn#add_transaction : I18n.btn#send_submit;

      let onSubmit = onSubmitAll->Option.getWithDefault(() => form.submit());

      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <ReactFlipToolkit.FlippedView flipId="form">
          <View style=FormStyles.header>
            <Typography.Headline>
              I18n.title#send->React.string
            </Typography.Headline>
            <Typography.Overline1 style=FormStyles.subtitle>
              I18n.title#send_many_transactions->React.string
            </Typography.Overline1>
            <Typography.Body2 style=FormStyles.subtitle>
              I18n.expl#send_many_transactions->React.string
            </Typography.Body2>
          </View>
          <FormGroupAmountWithTokenSelector
            label=I18n.label#send_amount
            value={form.values.amount}
            setValue={f =>
              form.setFieldValue(Amount, f(form.values.amount), ())
            }
            handleChange={form.handleChange(Amount)}
            error={form.getFieldError(Field(Amount))}
            selectedToken
            showSelector={!batchMode}
            setSelectedToken={newToken => setSelectedToken(_ => newToken)}
            ?token
          />
          <FormGroupAccountSelector
            disabled=batchMode
            label=I18n.label#send_sender
            value={form.values.sender}
            handleChange={form.handleChange(Sender)}
            error={form.getFieldError(Field(Sender))}
            ?token
          />
          <FormGroupContactSelector
            label=I18n.label#send_recipient
            filterOut={form.values.sender}
            value={form.values.recipient}
            handleChange={form.handleChange(Recipient)}
            error={form.getFieldError(Field(Recipient))}
          />
          <TouchableOpacity
            style=styles##advancedOptionButton
            activeOpacity=1.
            onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
            <Typography.Overline2>
              I18n.btn#advanced_options->React.string
            </Typography.Overline2>
            <ThemedSwitch value=advancedOptionOpened />
          </TouchableOpacity>
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView
          flipId="advancedOption"
          scale=false
          translate=advancedOptionOpened
          opacity=true>
          <ReactFlipToolkit.Flipper
            flipKey={advancedOptionOpened->string_of_bool}>
            {advancedOptionOpened
               ? <ReactFlipToolkit.FlippedView
                   flipId="innerAdvancedOption" onAppear onExit>
                   <SendViewAdvancedOptions
                     operation={simulatedTransaction(
                       mode,
                       batch,
                       form,
                       token,
                     )}
                     form
                   />
                 </ReactFlipToolkit.FlippedView>
               : React.null}
          </ReactFlipToolkit.Flipper>
        </ReactFlipToolkit.FlippedView>
        <ReactFlipToolkit.FlippedView flipId="submit">
          <View style=FormStyles.verticalFormAction>
            <Buttons.SubmitPrimary
              text=submitLabel
              onPress={_ => onSubmit()}
              loading
              disabledLook={!formFieldsAreValids}
            />
            {onAddToBatch->ReactUtils.mapOpt(addToBatch =>
               <Buttons.FormSecondary
                 style=styles##addTransaction
                 text=I18n.btn#start_batch_transaction
                 onPress={_ => addToBatch()}
               />
             )}
          </View>
        </ReactFlipToolkit.FlippedView>
      </>;
    };
  };
};

module EditionView = {
  [@react.component]
  let make = (~batch, ~initValues, ~onSubmit, ~token=?, ~index, ~loading) => {
    let (initValues, advancedOptionOpened) = initValues;

    let (advancedOptionOpened, _) as advancedOptionState =
      React.useState(_ => advancedOptionOpened);

    let form =
      Form.use(~initValues, None, token, onSubmit(advancedOptionOpened));

    <Form.View
      batch
      advancedOptionState
      tokenState=(None, _ => ())
      ?token
      form
      mode={Form.View.Edition(index)}
      loading
    />;
  };
};

[@react.component]
let make = (~closeAction) => {
  let account = StoreContext.SelectedAccount.useGet();
  let initToken = StoreContext.SelectedToken.useGet();

  let updateAccount = StoreContext.SelectedAccount.useSet();

  let (advancedOptionsOpened, setAdvancedOptions) as advancedOptionState =
    React.useState(_ => false);

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let (selectedToken, _) as tokenState =
    React.useState(_ => initToken->Option.map(initToken => initToken.address));
  let token = StoreContext.Tokens.useGet(selectedToken);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendTransfer = (transfer, password) => {
    let operation = SendForm.toOperation(transfer);

    let ((sourceAddress, _), _) = sourceDestination(transfer);

    sendOperation({operation, password})
    ->Future.tapOk(hash => {setModalStep(_ => SubmittedStep(hash))})
    ->Future.tapOk(_ => {updateAccount(sourceAddress)})
    ->ignore;
  };

  let (batch, setBatch) = React.useState(_ => []);

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let submitAction = React.useRef(`SubmitAll);

  let onSubmitBatch = batch => {
    let transaction = SendForm.buildTransaction(batch->List.reverse, token);
    sendOperationSimulate(SendForm.toSimulation(transaction))
    ->Future.tapOk(dryRun => {
        setModalStep(_ => PasswordStep(transaction, dryRun))
      })
    ->ignore;
  };

  let onSubmit = ({state, send}: SendForm.onSubmitAPI) =>
    switch (submitAction.current) {
    | `SubmitAll =>
      onSubmitBatch([(state.values, advancedOptionsOpened), ...batch])
    | `AddToBatch =>
      setBatch(l => [(state.values, advancedOptionsOpened), ...l]);
      setAdvancedOptions(_ => false);
      send(ResetForm);
      send(SetFieldValue(Sender, state.values.sender));
    };

  let form: SendForm.api = Form.use(account, token, onSubmit);

  let onSubmitAll = _ => {
    submitAction.current = `SubmitAll;
    form.submit();
  };

  let onAddToBatch = _ => {
    submitAction.current = `AddToBatch;
    form.submit();
    setModalStep(_ => BatchStep);
  };

  let onEdit = (i, advOpened, {state}: SendForm.onSubmitAPI) => {
    setBatch(b =>
      b->List.mapWithIndex((j, v) => i == j ? (state.values, advOpened) : v)
    );
    setModalStep(_ => BatchStep);
  };

  let onDelete = i => {
    setBatch(b => b->List.keepWithIndex((_, j) => j != i));
    List.length(batch) == 1 ? setModalStep(_ => SendStep) : ();
  };

  let closing =
    switch (form.formState, modalStep) {
    | (Pristine, _) when batch == [] => ModalFormView.Close(closeAction)
    | (_, SubmittedStep(_)) => ModalFormView.Close(closeAction)
    | _ =>
      ModalFormView.confirm(~actionText=I18n.btn#send_cancel, closeAction)
    };

  let back =
    switch (modalStep) {
    | PasswordStep(_, _) =>
      Some(() => setModalStep(_ => batch == [] ? SendStep : BatchStep))
    | EditStep(_, _) => Some(() => setModalStep(_ => BatchStep))
    | SendStep => batch != [] ? Some(_ => setModalStep(_ => BatchStep)) : None
    | _ => None
    };

  let onPressCancel = _ => {
    closeAction();
    Routes.(push(Operations));
  };

  let loadingSimulate = operationSimulateRequest->ApiRequest.isLoading;
  let loading = operationRequest->ApiRequest.isLoading;

  let showCurrency = {
    token->Option.mapWithDefault(I18n.t#xtz_amount, (token, a) =>
      I18n.t#amount(a, token.symbol)
    );
  };

  <ReactFlipToolkit.Flipper
    flipKey={advancedOptionsOpened->string_of_bool ++ modalStep->stepToString}>
    <ReactFlipToolkit.FlippedView flipId="modal">
      <ModalFormView back closing>
        <ReactFlipToolkit.FlippedView.Inverse inverseFlipId="modal">
          {switch (modalStep) {
           | SubmittedStep(hash) =>
             <SubmittedView
               hash
               onPressCancel
               submitText=I18n.btn#go_operations
             />
           | BatchStep =>
             <BatchView
               onAddTransfer={_ => setModalStep(_ => SendStep)}
               batch
               showCurrency
               reduceAmounts={reduceAmounts(token)}
               onSubmitBatch
               onEdit={(i, v) => setModalStep(_ => EditStep(i, v))}
               onDelete
               loading=loadingSimulate
             />
           | EditStep(index, initValues) =>
             let onSubmit = (advOpened, form) =>
               onEdit(index, advOpened, form);
             <EditionView
               batch
               initValues
               onSubmit
               ?token
               index
               loading=false
             />;
           | SendStep =>
             let onSubmit = batch != [] ? onAddToBatch : onSubmitAll;
             let onAddToBatch = batch != [] ? None : Some(onAddToBatch);
             <Form.View
               batch
               advancedOptionState
               tokenState
               ?token
               form
               mode={Form.View.Creation(onAddToBatch, onSubmit)}
               loading=loadingSimulate
             />;
           | PasswordStep(transfer, dryRun) =>
             let (source, destinations) = sourceDestination(transfer);
             <SignOperationView
               title=I18n.title#confirmation
               source
               destinations
               subtitle=I18n.expl#confirm_operation
               content={buildSummaryContent(transfer, dryRun)}
               showCurrency
               sendOperation={sendTransfer(transfer)}
               loading
             />;
           }}
        </ReactFlipToolkit.FlippedView.Inverse>
      </ModalFormView>
    </ReactFlipToolkit.FlippedView>
  </ReactFlipToolkit.Flipper>;
};
