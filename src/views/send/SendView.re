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

type transfer =
  | ProtocolTransaction(Protocol.transaction)
  | TokenTransfer(Token.operation, int, string, string);

let toOperation = t =>
  switch (t) {
  | ProtocolTransaction(transaction) => Operation.transaction(transaction)
  | TokenTransfer(operation, _, _, _) => Operation.Token(operation)
  };

let buildTransfer =
    (
      values: SendForm.StateLenses.state,
      advancedOptionOpened: bool,
      token: option(Token.t),
    ) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  let source = values.sender;
  let amount = values.amount->Js.Float.fromString;
  let destination = values.recipient;
  let fee = values.fee->mapIfAdvanced(Js.Float.fromString);
  let counter = values.counter->mapIfAdvanced(int_of_string);
  let gasLimit = values.gasLimit->mapIfAdvanced(int_of_string);
  let storageLimit = values.storageLimit->mapIfAdvanced(int_of_string);
  let forceLowFee =
    advancedOptionOpened && values.forceLowFee ? Some(true) : None;

  switch (token) {
  | Some(token) =>
    let amount = amount->int_of_float;
    let transfer =
      Token.makeTransfer(
        ~source,
        ~amount,
        ~destination,
        ~contract=token.address,
        ~fee?,
        ~counter?,
        ~gasLimit?,
        ~storageLimit?,
        ~forceLowFee?,
        (),
      );
    TokenTransfer(transfer, amount, source, destination);
  | None =>
    let t =
      Protocol.makeSingleTransaction(
        ~source,
        ~amount,
        ~destination,
        ~fee?,
        ~counter?,
        ~gasLimit?,
        ~storageLimit?,
        ~forceLowFee?,
        (),
      );
    ProtocolTransaction(t);
  };
};

let buildTransaction =
    (
      batch: list(SendForm.StateLenses.state),
      advancedOptionOpened: bool,
      token: option(Token.t),
    ) => {
  switch (batch) {
  | [] => assert(false)
  | [transfer] => buildTransfer(transfer, advancedOptionOpened, token)
  | [first, ..._] as transfers =>
    let mapIfAdvanced = (v, map) =>
      advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

    let source = first.sender;
    let forceLowFee = first.forceLowFee ? Some(true) : None;
    let counter = first.counter->mapIfAdvanced(int_of_string);

    let transfers =
      transfers->Belt.List.map((t: SendForm.StateLenses.state) => {
        let amount = t.amount->Js.Float.fromString;
        let destination = t.recipient;
        let gasLimit = t.gasLimit->mapIfAdvanced(int_of_string);
        let storageLimit = t.storageLimit->mapIfAdvanced(int_of_string);
        let fee = t.fee->mapIfAdvanced(Js.Float.fromString);
        Protocol.makeTransfer(
          ~amount,
          ~destination,
          ~fee?,
          ~gasLimit?,
          ~storageLimit?,
          (),
        );
      });

    Protocol.makeTransaction(
      ~source,
      ~transfers,
      ~counter?,
      ~forceLowFee?,
      (),
    )
    ->ProtocolTransaction;
  };
};

type step =
  | SendStep
  | PasswordStep(transfer, Protocol.simulationResults)
  | BatchStep;

let sourceDestination = transfer => {
  switch (transfer) {
  | ProtocolTransaction({source, transfers: [t]}) => (source, t.destination)
  | TokenTransfer(_, _, source, destination) => (source, destination)
  };
};

let buildSummaryContent =
    (transfer, token, dryRun: Protocol.simulationResults) => {
  switch (transfer) {
  | ProtocolTransaction({transfers: [{amount}]}) =>
    let subtotal = (
      I18n.label#summary_subtotal,
      I18n.t#xtz_amount(amount->Js.Float.toFixedWithPrecision(~digits=1)),
    );
    let total = amount +. dryRun.fee;
    let total = (
      I18n.label#summary_total,
      I18n.t#xtz_amount(total->Js.Float.toString),
    );
    let fee = (
      I18n.label#fee,
      I18n.t#xtz_amount(dryRun.fee->Js.Float.toString),
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
  let build = (initAccount: option(Account.t), onSubmit) => {
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
      ~initialState={
        amount: "",
        sender: initAccount->Belt.Option.mapWithDefault("", a => a.address),
        recipient: "",
        fee: "",
        counter: "",
        gasLimit: "",
        storageLimit: "",
        forceLowFee: false,
      },
      (),
    );
  };

  module View = {
    open SendForm;

    [@react.component]
    let make =
        (
          ~batch,
          ~advancedOptionState,
          ~tokenState,
          ~token=?,
          ~onAddToBatch,
          ~onSubmitAll,
          ~setBatchStep,
          ~form,
        ) => {
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;
      let theme = ThemeContext.useTheme();

      let batchMode = batch != [];
      let submitLabel =
        batchMode ? I18n.btn#batch_submit : I18n.btn#send_submit;

      <>
        {<TouchableOpacity
           onPress={_ => setBatchStep()} style=FormStyles.topLeftButton>
           <Icons.List size=36. color={theme.colors.iconMediumEmphasis} />
         </TouchableOpacity>}
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
             ? <SendViewAdvancedOptions form ?token /> : React.null}
        </View>
        <View style=FormStyles.verticalFormAction>
          <Buttons.SubmitPrimary text=submitLabel onPress=onSubmitAll />
          {<Buttons.FormSecondary
             style=styles##addTransaction
             text=I18n.btn#send_another_transaction
             onPress=onAddToBatch
           />
           ->ReactUtils.onlyWhen(token == None)}
        </View>
      </>;
    };
  };
};

[@react.component]
let make = (~onPressCancel) => {
  let account = StoreContext.SelectedAccount.useGet();
  let initToken = StoreContext.SelectedToken.useGet();

  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (selectedToken, _) as tokenState =
    React.useState(_ =>
      initToken->Belt.Option.map(initToken => initToken.address)
    );
  let token = StoreContext.Tokens.useGet(selectedToken);

  let (operationRequest, sendOperation) = StoreContext.Operations.useCreate();

  let sendTransfer = (transfer, password) => {
    let operation = toOperation(transfer);
    sendOperation({operation, password})->ignore;
  };

  let (batch, setBatch) = React.useState(_ => []);

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let submitAction = React.useRef(`SubmitAll);

  let onSubmitBatch = batch => {
    let transaction = buildTransaction(batch, advancedOptionOpened, token);
    sendOperationSimulate(toOperation(transaction))
    ->Future.tapOk(dryRun => {
        setModalStep(_ => PasswordStep(transaction, dryRun))
      })
    ->ignore;
  };

  let onSubmit = ({state, send}: SendForm.onSubmitAPI) =>
    switch (submitAction.current) {
    | `SubmitAll => onSubmitBatch([state.values, ...batch])
    | `AddToBatch =>
      setBatch(l => [state.values, ...l]);
      send(ResetForm);
    };

  let form = Form.build(account, onSubmit);

  let onSubmitAll = _ => {
    submitAction.current = `SubmitAll;
    form.submit();
  };

  let onAddToBatch = _ => {
    submitAction.current = `AddToBatch;
    form.submit();
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
     | Done(Ok((hash, _)), _) =>
       <>
         <Typography.Headline style=FormStyles.header>
           I18n.title#operation_injected->React.string
         </Typography.Headline>
         <Typography.Overline2>
           I18n.t#operation_hash->React.string
         </Typography.Overline2>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
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
             back={_ => setModalStep(_ => SendStep)}
             batch
             onSubmitBatch
           />
         | SendStep =>
           <Form.View
             batch
             advancedOptionState
             tokenState
             ?token
             form
             setBatchStep={_ => setModalStep(_ => BatchStep)}
             onAddToBatch
             onSubmitAll
           />
         | PasswordStep(transfer, dryRun) =>
           let (source, destination) = sourceDestination(transfer);
           <SignOperationView
             title=I18n.title#confirmation
             source=(source, I18n.title#sender_account)
             destination=(destination, I18n.title#recipient_account)
             subtitle=I18n.expl#confirm_operation
             onPressCancel={_ => setModalStep(_ => SendStep)}
             content={buildSummaryContent(transfer, token, dryRun)}
             sendOperation={sendTransfer(transfer)}
           />;
         };
       }
     }}
  </ModalView.Form>;
};
