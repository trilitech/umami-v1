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
      ) => {
    let decoration =
      token->Belt.Option.map(token => tokenDecoration(~symbol=token.symbol));
    <>
      <TokenSelector
        selectedToken
        setSelectedToken
        style=styles##tokenSelector
      />
      <FormGroupXTZInput
        label
        value
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
  | ProtocolTransfer(Protocol.transfer, list(Protocol.transfer))
  | TokenTransfer(Token.operation, int, string, string);

let toOperation = t =>
  switch (t) {
  | ProtocolTransfer(operation, _) => Operation.transfer(operation)
  | TokenTransfer(operation, _, _, _) => Operation.Token(operation)
  };

let buildTransfer =
    (
      state: SendForm.state,
      advancedOptionOpened: bool,
      token: option(Token.t),
    ) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  let source = state.values.sender;
  let amount = state.values.amount->Js.Float.fromString;
  let destination = state.values.recipient;
  let fee = state.values.fee->mapIfAdvanced(Js.Float.fromString);
  let counter = state.values.counter->mapIfAdvanced(int_of_string);
  let gasLimit = state.values.gasLimit->mapIfAdvanced(int_of_string);
  let storageLimit = state.values.storageLimit->mapIfAdvanced(int_of_string);
  let forceLowFee =
    advancedOptionOpened && state.values.forceLowFee ? Some(true) : None;

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
    let transfer =
      Protocol.makeTransfer(
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
    ProtocolTransfer(transfer, []);
  };
};

type step =
  | SendStep
  | PasswordStep(transfer, Protocol.simulationResults);

let sourceDestination = transfer => {
  switch (transfer) {
  | ProtocolTransfer({source, destination}, _) => (source, destination)
  | TokenTransfer(_, _, source, destination) => (source, destination)
  };
};

let buildSummaryContent =
    (transfer, token, dryRun: Protocol.simulationResults) => {
  switch (transfer) {
  | ProtocolTransfer({amount}, _) =>
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
  let build =
      (initAccount: option(Account.t), advancedOptionOpened, token, onSubmit) => {
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
        ({state}) => {
          let operation = buildTransfer(state, advancedOptionOpened, token);
          onSubmit(operation);

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
    let make = (~advancedOptionState, ~tokenState, ~token=?, ~form) => {
      let onSubmitSendForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;

      <>
        <Typography.Headline style=FormStyles.header>
          I18n.title#send->React.string
        </Typography.Headline>
        <FormGroupAmountWithTokenSelector
          label=I18n.label#send_amount
          value={form.values.amount}
          handleChange={form.handleChange(Amount)}
          error={form.getFieldError(Field(Amount))}
          selectedToken
          setSelectedToken={newToken => setSelectedToken(_ => newToken)}
          ?token
        />
        <FormGroupAccountSelector
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
          <Buttons.SubmitPrimary
            text=I18n.btn#send_submit
            onPress=onSubmitSendForm
          />
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

  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let form = {
    let onSubmit = transfer =>
      sendOperationSimulate(toOperation(transfer))
      ->Future.tapOk(dryRun => {
          setModalStep(_ => PasswordStep(transfer, dryRun))
        })
      ->ignore;
    Form.build(account, advancedOptionOpened, token, onSubmit);
  };

  let closing =
    switch (form.formState) {
    | Pristine => ModalView.Close(_ => onPressCancel())
    | _ =>
      ModalView.confirm(~actionText=I18n.btn#send_cancel, _ => onPressCancel())
    };

  let onPressCancel = _ => onPressCancel();

  <ModalView.Form closing>
    {switch (modalStep, operationRequest, operationSimulateRequest) {
     | (_, Done(Ok((hash, _)), _), _) =>
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
     | (_, Done(Error(error), _), _) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Loading(_), _) =>
       <ModalView.LoadingView title=I18n.title#submitting />
     | (_, _, Loading(_)) =>
       <ModalView.LoadingView title=I18n.title#simulation />
     | (SendStep, _, _) =>
       <Form.View advancedOptionState tokenState ?token form />
     | (PasswordStep(transfer, dryRun), _, _) =>
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
     }}
  </ModalView.Form>;
};
