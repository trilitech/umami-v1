open ReactNative;

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
      "switchCmp": style(~height=16.->dp, ~width=32.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
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

let buildTransaction = (state: SendForm.state, advancedOptionOpened) => {
  let mapIfAdvanced = (v, map) =>
    advancedOptionOpened && v->Js.String2.length > 0 ? Some(v->map) : None;

  Injection.makeTransfer(
    ~source=state.values.sender,
    ~amount=state.values.amount->Js.Float.fromString,
    ~destination=state.values.recipient,
    ~fee=?state.values.fee->mapIfAdvanced(Js.Float.fromString),
    ~counter=?state.values.counter->mapIfAdvanced(int_of_string),
    ~gasLimit=?state.values.gasLimit->mapIfAdvanced(int_of_string),
    ~storageLimit=?state.values.storageLimit->mapIfAdvanced(int_of_string),
    ~forceLowFee=?
      advancedOptionOpened && state.values.forceLowFee ? Some(true) : None,
    (),
  );
};

type step =
  | SendStep
  | PasswordStep(Injection.operation);

module Form = {
  let build =
      (initAccount: option(Account.t), advancedOptionOpened, onSubmit) => {
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
          let operation = buildTransaction(state, advancedOptionOpened);
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
    let make = (~onPressCancel, ~advancedOptionState, ~form) => {
      let onSubmitSendForm = _ => {
        form.submit();
      };
      let (advancedOptionOpened, setAdvancedOptionOpened) = advancedOptionState;

      <>
        <Typography.Headline2 style=styles##title>
          I18n.title#send->React.string
        </Typography.Headline2>
        <FormGroupXTZInput
          label=I18n.label#send_amount
          value={form.values.amount}
          handleChange={form.handleChange(Amount)}
          error={form.getFieldError(Field(Amount))}
        />
        <FormGroupAccountSelector
          label=I18n.label#send_sender
          value={form.values.sender}
          handleChange={form.handleChange(Sender)}
          error={form.getFieldError(Field(Sender))}
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
            <Typography.Overline1>
              I18n.btn#advanced_options->React.string
            </Typography.Overline1>
            <SwitchNative
              value=advancedOptionOpened
              //onValueChange=handleChange
              thumbColor="#000"
              trackColor={Switch.trackColor(
                ~_true="#FFF",
                ~_false="rgba(255,255,255,0.5)",
                (),
              )}
              style=styles##switchCmp
              thumbStyle=styles##switchThumb
            />
          </TouchableOpacity>
          {advancedOptionOpened ? <SendViewAdvancedOptions form /> : React.null}
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
  let account = StoreContext.useAccount();

  let (advancedOptionOpened, _) as advancedOptionState =
    React.useState(_ => false);

  let (operationRequest, sendOperation) = OperationApiRequest.useCreate();

  let sendOperation = (operation, ~password) =>
    sendOperation(OperationApiRequest.{operation, password})->ignore;

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let form =
    Form.build(account, advancedOptionOpened, op =>
      setModalStep(_ => PasswordStep(op))
    );

  React.useEffect0(() => {None});

  <ModalView.Form>
    {switch (modalStep, operationRequest) {
     | (_, Done(Ok(hash))) =>
       <>
         <Typography.Headline2 style=styles##title>
           I18n.title#operation_injected->React.string
         </Typography.Headline2>
         <Typography.Overline1>
           I18n.t#operation_hash->React.string
         </Typography.Overline1>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error))) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | (_, Loading) =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color=Colors.highIcon
         />
       </View>
     | (SendStep, _) => <Form.View onPressCancel advancedOptionState form />
     | (PasswordStep(operation), _) =>
       <SignOperationView
         onPressCancel={_ => setModalStep(_ => SendStep)}
         operation
         sendOperation
       />
     }}
  </ModalView.Form>;
};
