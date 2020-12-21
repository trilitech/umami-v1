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
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "formAction":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~marginTop=24.->dp,
          (),
        ),
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
      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

let buildTransaction =
    (
      state: SendForm.state,
      advancedOptionOpened: bool,
      token: option(Token.t),
    ) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  switch (token) {
  | Some(token) =>
    SendForm.TokensOperation(
      Token.makeTransfer(
        ~source=state.values.sender,
        ~amount=state.values.amount->Js.Float.fromString->int_of_float,
        ~destination=state.values.recipient,
        ~contract=token.address,
        ~fee=?state.values.fee->mapIfAdvanced(Js.Float.fromString),
        ~counter=?state.values.counter->mapIfAdvanced(int_of_string),
        ~gasLimit=?state.values.gasLimit->mapIfAdvanced(int_of_string),
        ~storageLimit=?
          state.values.storageLimit->mapIfAdvanced(int_of_string),
        ~forceLowFee=?
          advancedOptionOpened && state.values.forceLowFee
            ? Some(true) : None,
        (),
      ),
      token,
    )
  | None =>
    SendForm.InjectionOperation(
      Injection.makeTransfer(
        ~source=state.values.sender,
        ~amount=state.values.amount->Js.Float.fromString,
        ~destination=state.values.recipient,
        ~fee=?state.values.fee->mapIfAdvanced(Js.Float.fromString),
        ~counter=?state.values.counter->mapIfAdvanced(int_of_string),
        ~gasLimit=?state.values.gasLimit->mapIfAdvanced(int_of_string),
        ~storageLimit=?
          state.values.storageLimit->mapIfAdvanced(int_of_string),
        ~forceLowFee=?
          advancedOptionOpened && state.values.forceLowFee
            ? Some(true) : None,
        (),
      ),
    )
  };
};

type step =
  | SendStep
  | PasswordStep(SendForm.operation);

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
          let operation =
            buildTransaction(state, advancedOptionOpened, token);
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
    let make =
        (~onPressCancel, ~advancedOptionState, ~tokenState, ~token=?, ~form) => {
      let onSubmitSendForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;
      let (selectedToken, setSelectedToken) = tokenState;

      <>
        <Typography.Headline style=styles##title>
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
        <View style=styles##formAction>
          <FormButton text=I18n.btn#cancel onPress=onPressCancel />
          <FormButton text=I18n.btn#ok onPress=onSubmitSendForm />
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

  let sendOperation = (operation, ~password) =>
    switch (operation) {
    | SendForm.InjectionOperation(operation) =>
      sendOperation(OperationApiRequest.regular(operation, password))->ignore
    | SendForm.TokensOperation(operation, _) =>
      sendOperation(OperationApiRequest.token(operation, password))->ignore
    };

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let form =
    Form.build(account, advancedOptionOpened, token, op =>
      setModalStep(_ => PasswordStep(op))
    );

  React.useEffect0(() => {None});

  let theme = ThemeContext.useTheme();

  <ModalView.Form>
    {switch (modalStep, operationRequest) {
     | (_, Done(Ok((hash, _)), _)) =>
       <>
         <Typography.Headline style=styles##title>
           I18n.title#operation_injected->React.string
         </Typography.Headline>
         <Typography.Overline2>
           I18n.t#operation_hash->React.string
         </Typography.Overline2>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error), _)) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Loading(_)) =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color={theme.colors.iconMediumEmphasis}
         />
       </View>
     | (SendStep, _) =>
       <Form.View onPressCancel advancedOptionState tokenState ?token form />
     | (PasswordStep(operation), _) =>
       <SignOperationView
         onPressCancel={_ => setModalStep(_ => SendStep)}
         operation
         sendOperation
       />
     }}
  </ModalView.Form>;
};
