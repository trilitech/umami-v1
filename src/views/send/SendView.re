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

let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN ? Error("not a float") : Valid;
  fieldState;
};

let isSomeAccount = value => {
  let fieldState: ReSchema.fieldState =
    value->Belt.Option.isNone ? Error("account needed") : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0
    || value->int_of_string_opt->Belt.Option.isSome
      ? Valid : Error("not an int");
  fieldState;
};

type transaction = {
  op: TezosClient.Injection.transaction,
  sender: Account.t,
  recipient: Account.t,
};

type step =
  | SendStep
  | PasswordStep(transaction);

[@react.component]
let make = (~onPressCancel) => {
  let account = StoreContext.useAccount();

  let (advancedOptionOpened, setAdvancedOptionOpened) =
    React.useState(_ => false);

  let (operationRequest, sendOperation) =
    OperationApiRequest.useCreateOperation();

  let (modalStep, setModalStep) = React.useState(_ => SendStep);

  let buildTransaction = (state: SendForm.state, sender, recipient) =>
    Injection.{
      source: sender.Account.address,
      amount: state.values.amount->Js.Float.fromString,
      destination: recipient.Account.address,
      fee:
        advancedOptionOpened && state.values.fee->Js.String2.length > 0
          ? Some(state.values.fee->Js.Float.fromString) : None,
      counter:
        advancedOptionOpened && state.values.counter->Js.String2.length > 0
          ? Some(state.values.counter->int_of_string) : None,
      gasLimit:
        advancedOptionOpened && state.values.gasLimit->Js.String2.length > 0
          ? Some(state.values.gasLimit->int_of_string) : None,
      storageLimit:
        advancedOptionOpened
        && state.values.storageLimit->Js.String2.length > 0
          ? Some(state.values.storageLimit->int_of_string) : None,
      forceLowFee:
        advancedOptionOpened && state.values.forceLowFee ? Some(true) : None,
      burnCap: None,
      confirmations: None,
    };

  let form: SendForm.api =
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            nonEmpty(Amount)
            + custom(values => isValidFloat(values.amount), Amount)
            + custom(values => isSomeAccount(values.sender), Sender)
            + custom(values => isSomeAccount(values.recipient), Sender)
            + custom(values => isValidFloat(values.fee), Fee)
            + custom(values => isValidInt(values.counter), Counter)
            + custom(values => isValidInt(values.gasLimit), GasLimit)
            + custom(values => isValidInt(values.storageLimit), StorageLimit),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          switch (state.values.sender, state.values.recipient) {
          | (Some(sender), Some(recipient)) =>
            let op = buildTransaction(state, sender, recipient);
            setModalStep(_ => PasswordStep({op, sender, recipient}));
            None;
          | _ => None
          }
        },
      ~initialState={
        amount: "",
        sender: account,
        recipient: None,
        fee: "",
        counter: "",
        gasLimit: "",
        storageLimit: "",
        forceLowFee: false,
      },
      (),
    );

  let passwordForm: SendForm.Password.api =
    SendForm.Password.use(
      ~schema={
        SendForm.Password.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=({state: _}) => {None},
      ~initialState={password: ""},
      (),
    );

  let onSubmitSendForm = _ => {
    form.submit();
  };

  let onSubmitAll = (operation, _) => {
    // checking password
    // getting stored data
    passwordForm.submit();
    sendOperation(Injection.Transaction(operation.op));
  };

  let sendFormView = () => {
    <>
      <Typography.Headline2 style=styles##title>
        "Send"->React.string
      </Typography.Headline2>
      <FormGroupTextInput
        label="Amount"
        value={form.values.amount}
        handleChange={form.handleChange(Amount)}
        error={form.getFieldError(Field(Amount))}
        keyboardType=`numeric
      />
      <FormGroupAccountSelector
        label="Sender account"
        value={form.values.sender}
        handleChange={form.handleChange(Sender)}
        error={form.getFieldError(Field(Sender))}
      />
      <FormGroupContactSelector
        label="Recipient account"
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
            "Advanced options"->React.string
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
        <FormButton text="CANCEL" onPress=onPressCancel />
        <FormButton
          text="OK"
          onPress=onSubmitSendForm
          disabled={form.formState == Errored}
        />
      </View>
    </>;
  };

  let passwordFormView = (~operation) => {
    <>
      <View style=styles##title>
        <Typography.Headline2>
          {Js.Float.toFixedWithPrecision(
             operation.op.Injection.amount,
             ~digits=1,
           )
           ->React.string}
          " XTZ"->React.string
        </Typography.Headline2>
        {operation.op.Injection.fee
         ->Belt.Option.mapWithDefault(React.null, fee =>
             <Typography.Body1 colorStyle=`mediumEmphasis>
               "+ Fee "->React.string
               {fee->Js.Float.toString->React.string}
               " XTZ"->React.string
             </Typography.Body1>
           )}
      </View>
      <OperationSummaryView
        style=styles##operationSummary
        sender={operation.sender}
        recipient={operation.recipient}
      />
      <FormGroupTextInput
        label="Password"
        value={passwordForm.values.password}
        handleChange={passwordForm.handleChange(Password)}
        error={passwordForm.getFieldError(Field(Password))}
        textContentType=`password
        secureTextEntry=true
      />
      <View style=styles##formAction>
        <FormButton text="CANCEL" onPress={_ => setModalStep(_ => SendStep)} />
        <FormButton
          text="SEND"
          onPress={operation->onSubmitAll}
          disabled={form.formState == Errored}
        />
      </View>
    </>;
  };

  <ModalView>
    {switch (modalStep, operationRequest) {
     | (_, Done(Ok(hash))) =>
       <>
         <Typography.Headline2 style=styles##title>
           "Operation injected in the node"->React.string
         </Typography.Headline2>
         <Typography.Overline1>
           "Operation hash"->React.string
         </Typography.Overline1>
         <Typography.Body1> hash->React.string </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | (_, Done(Error(error))) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
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
     | (SendStep, _) => sendFormView()
     | (PasswordStep(operation), _) => passwordFormView(~operation)
     }}
  </ModalView>;
};
