open ReactNative;

module FormGroupAmountWithTokenSelector = {
  let styles =
    Style.(
      StyleSheet.create({
        "tokenSelector": style(~marginBottom=10.->dp, ~alignSelf=`auto, ()),
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
        ~style: option(ReactNative.Style.t)=?,
        ~selectedToken,
        ~setSelectedToken,
        ~token: option(Token.t)=?,
        ~showSelector,
        ~setValue=?,
      ) => {
    let decoration =
      token->Belt.Option.map(token => tokenDecoration(~symbol=token.symbol));
    <>
      {<TokenSelector
         selectedToken
         setSelectedToken
         style=styles##tokenSelector
       />
       ->ReactUtils.onlyWhen(showSelector)}
      <FormGroupXTZInput
        label
        value
        ?setValue
        handleChange
        error
        ?decoration
        ?style
        ?token
      />
    </>;
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
  | BatchStep;

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
      transfers->Belt.List.map(t =>
        (
          None,
          (
            t.destination,
            t.amount->Js.Float.toFixedWithPrecision(~digits=6),
          ),
        )
      );
    ((source, sourceLbl), `Many(destinations));
  | TokenTransfer(_, _, source, destination) => (
      (source, sourceLbl),
      `One((destination, recipientLbl)),
    )
  };
};

let buildSummaryContent =
    (
      transaction: SendForm.transaction,
      token,
      dryRun: Protocol.simulationResults,
    ) => {
  switch (transaction) {
  | ProtocolTransaction({transfers}) =>
    let amount =
      transfers->Belt.List.reduce(0., (acc, {amount}) => acc +. amount);
    let subtotal = (
      I18n.label#summary_subtotal,
      I18n.t#xtz_amount(amount->Js.Float.toFixedWithPrecision(~digits=6)),
    );
    let total = amount +. dryRun.fee;
    let total = (
      I18n.label#summary_total,
      I18n.t#xtz_amount(total->Js.Float.toFixedWithPrecision(~digits=6)),
    );
    let fee = (
      I18n.label#fee,
      I18n.t#xtz_amount(
        dryRun.fee->Js.Float.toFixedWithPrecision(~digits=6),
      ),
    );
    [subtotal, fee, total];
  | TokenTransfer(_, amount, _source, _destination) =>
    let fee = (
      I18n.label#fee,
      I18n.t#xtz_amount(dryRun.fee->Js.Float.toString),
    );
    let amount =
      token->Belt.Option.mapWithDefault("", (Token.{symbol}) =>
        I18n.t#amount(amount->Js.Int.toString, symbol)
      );
    let amount = (I18n.label#send_amount, amount);
    [amount, fee];
  };
};

module Form = {
  let defaultInit = (account: option(Account.t)) =>
    SendForm.StateLenses.{
      amount: "",
      sender: account->Belt.Option.mapWithDefault("", a => a.address),
      recipient: "",
      fee: "",
      counter: "",
      gasLimit: "",
      storageLimit: "",
      forceLowFee: false,
      dryRun: None,
    };

  let use = (~initValues=?, initAccount, onSubmit) => {
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            nonEmpty(Amount)
            + nonEmpty(Sender)
            + nonEmpty(Recipient)
            + custom(
                values => FormUtils.isValidFloat(values.amount),
                Amount,
              )
            + custom(values => FormUtils.isValidFloat(values.fee), Fee)
            + custom(
                values => FormUtils.isValidInt(values.counter),
                Counter,
              )
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
        initValues->Belt.Option.getWithDefault(defaultInit(initAccount)),
      (),
    );
  };

  module View = {
    open SendForm;

    type mode =
      | Edition(int, unit => unit)
      | Creation(option(unit => unit), unit => unit, option(unit => unit));

    let simulateTransaction = (mode, batch, form: SendForm.api, token) => {
      let (batch, index) =
        switch (mode) {
        | Edition(index, _) =>
          let batch =
            batch
            ->Belt.List.mapWithIndex((id, elt) =>
                id == index ? (form.state.values, true) : elt
              )
            ->Belt.List.reverse;
          (batch, Some(batch->List.length - (index + 1)));

        | Creation(_) =>
          let batch =
            [(form.state.values, true), ...batch]->Belt.List.reverse;
          let length = batch->Belt.List.length;

          (batch, Some(length - 1));
        };

      SendForm.buildTransaction(batch, token)
      |> SendForm.toSimulation(~index?);
    };

    [@react.component]
    let make =
        (~batch, ~advancedOptionState, ~tokenState, ~token=?, ~mode, ~form) => {
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;
      let theme = ThemeContext.useTheme();

      let (editing, back, onAddToBatch, onSubmitAll, batchMode) =
        switch (mode) {
        | Edition(_, back) => (true, Some(back), None, None, false)
        | Creation(batch, submit, back) => (
            false,
            back,
            batch,
            Some(submit),
            batch == None,
          )
        };

      let submitLabel =
        editing
          ? I18n.btn#update
          : batchMode ? I18n.btn#add_transaction : I18n.btn#send_submit;

      let onSubmit =
        onSubmitAll->Belt.Option.getWithDefault(() => form.submit());

      <>
        {back->ReactUtils.mapOpt(back =>
           <TouchableOpacity
             onPress={_ => back()} style=FormStyles.topLeftButton>
             <Icons.ArrowLeft
               size=36.
               color={theme.colors.iconMediumEmphasis}
             />
           </TouchableOpacity>
         )}
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
          value={form.values.recipient}
          handleChange={form.handleChange(Recipient)}
          error={form.getFieldError(Field(Recipient))}
        />
        <View>
          <TouchableOpacity
            style=styles##advancedOptionButton
            activeOpacity=1.
            onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
            <Typography.Overline2>
              I18n.btn#advanced_options->React.string
            </Typography.Overline2>
            <ThemedSwitch value=advancedOptionOpened />
          </TouchableOpacity>
          {advancedOptionOpened
             ? <SendViewAdvancedOptions
                 operation={simulateTransaction(mode, batch, form, token)}
                 form
               />
             : React.null}
        </View>
        <View style=FormStyles.verticalFormAction>
          <Buttons.SubmitPrimary text=submitLabel onPress={_ => onSubmit()} />
          {onAddToBatch
           ->ReactUtils.mapOpt(addToBatch =>
               <Buttons.FormSecondary
                 style=styles##addTransaction
                 text=I18n.btn#start_batch_transaction
                 onPress={_ => addToBatch()}
               />
             )
           ->ReactUtils.onlyWhen(token == None)}
        </View>
      </>;
    };
  };
};

module EditionView = {
  [@react.component]
  let make = (~batch, ~initValues, ~onSubmit, ~index, ~back) => {
    let (initValues, advancedOptionOpened) = initValues;

    let (advancedOptionOpened, _) as advancedOptionState =
      React.useState(_ => advancedOptionOpened);

    let form = Form.use(~initValues, None, onSubmit(advancedOptionOpened));

    <Form.View
      batch
      advancedOptionState
      tokenState=(None, _ => ())
      form
      mode={Form.View.Edition(index, back)}
    />;
  };
};

module SubmittedView = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~alignItems=`center, ()),
        "title": style(~marginBottom=6.->dp, ()),
        "body": style(~marginBottom=30.->dp, ~textAlign=`center, ()),
        "hashTitle": style(~marginBottom=2.->dp, ()),
        "addressContainer":
          style(
            ~flexDirection=`row,
            ~justifyContent=`center,
            ~alignItems=`center,
            (),
          ),
        "address": style(~marginHorizontal=4.->dp, ()),
      })
    );

  [@react.component]
  let make = (~hash, ~onPressCancel) => {
    let addToast = LogsContext.useToast();

    <View style=styles##container>
      <Typography.Headline style=styles##title>
        I18n.title#operation_submited->React.string
      </Typography.Headline>
      <Typography.Body2 colorStyle=`mediumEmphasis style=styles##body>
        I18n.expl#operation->React.string
      </Typography.Body2>
      <Typography.Overline2 style=styles##hashTitle>
        I18n.t#operation_hash->React.string
      </Typography.Overline2>
      <View style=styles##addressContainer>
        <Typography.Address fontSize=16. style=styles##address>
          hash->React.string
        </Typography.Address>
        <ClipboardButton copied=I18n.log#address addToast data=hash />
      </View>
      <View style=FormStyles.formAction>
        <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
      </View>
    </View>;
  };
};

[@react.component]
let make = (~onPressCancel) => {
  let account = StoreContext.SelectedAccount.useGet();
  let initToken = StoreContext.SelectedToken.useGet();

  let (advancedOptionsOpened, setAdvancedOptions) as advancedOptionState =
    React.useState(_ => false);

  let (selectedToken, _) as tokenState =
    React.useState(_ =>
      initToken->Belt.Option.map(initToken => initToken.address)
    );
  let token = StoreContext.Tokens.useGet(selectedToken);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendTransfer = (transfer, password) => {
    let operation = SendForm.toOperation(transfer);
    sendOperation({operation, password})->ignore;
  };

  let (batch, setBatch) = React.useState(_ => []);

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let submitAction = React.useRef(`SubmitAll);

  let onSubmitBatch = batch => {
    let transaction =
      SendForm.buildTransaction(batch->Belt.List.reverse, token);
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

  let form: SendForm.api = Form.use(account, onSubmit);

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
      b->Belt.List.mapWithIndex((j, v) =>
        i == j ? (state.values, advOpened) : v
      )
    );
    setModalStep(_ => BatchStep);
  };

  let onDelete = i => {
    setBatch(b => b->Belt.List.keepWithIndex((_, j) => j != i));
    Belt.List.length(batch) == 1 ? setModalStep(_ => SendStep) : ();
  };

  let closing =
    switch (form.formState) {
    | Pristine when batch == [] => ModalView.Close(_ => onPressCancel())
    | _ =>
      ModalView.confirm(~actionText=I18n.btn#send_cancel, _ => onPressCancel())
    };

  let onPressCancel = _ => onPressCancel();

  <ModalView.Form closing>
    {switch (operationRequest) {
     | Done(Ok((hash, _)), _) => <SubmittedView hash onPressCancel />
     | Done(Error(error), _) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | Loading(_) => <ModalView.LoadingView title=I18n.title#submitting />
     | NotAsked =>
       if (ApiRequest.isLoading(operationSimulateRequest)) {
         <ModalView.LoadingView title=I18n.title#simulation />;
       } else {
         switch (modalStep) {
         | BatchStep =>
           <BatchView
             onAddTransfer={_ => setModalStep(_ => SendStep)}
             batch
             onSubmitBatch
             onEdit={(i, v) => setModalStep(_ => EditStep(i, v))}
             onDelete
           />
         | EditStep(index, initValues) =>
           let onSubmit = (advOpened, form) =>
             onEdit(index, advOpened, form);
           let back = () => setModalStep(_ => BatchStep);
           <EditionView batch initValues onSubmit index back />;
         | SendStep =>
           let back =
             batch != [] ? Some(_ => setModalStep(_ => BatchStep)) : None;
           let onSubmit = batch != [] ? onAddToBatch : onSubmitAll;
           let onAddToBatch = batch != [] ? None : Some(onAddToBatch);
           <Form.View
             batch
             advancedOptionState
             tokenState
             ?token
             form
             mode={Form.View.Creation(onAddToBatch, onSubmit, back)}
           />;
         | PasswordStep(transfer, dryRun) =>
           let (source, destinations) = sourceDestination(transfer);
           <SignOperationView
             title=I18n.title#confirmation
             source
             destinations
             subtitle=I18n.expl#confirm_operation
             back={() => setModalStep(_ => batch == [] ? SendStep : BatchStep)}
             content={buildSummaryContent(transfer, token, dryRun)}
             sendOperation={sendTransfer(transfer)}
           />;
         };
       }
     }}
  </ModalView.Form>;
};
