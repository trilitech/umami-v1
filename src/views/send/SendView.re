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
      "chevron":
        style(
          ~color=Theme.colorDarkMediumEmphasis,
          ~fontSize=14.,
          ~fontWeight=`_600,
          ~transform=[|scaleY(~scaleY=1.65), rotate(~rotate=(-90.)->deg)|],
          (),
        ),
      "chevronOpened": style(~transform=[|scaleX(~scaleX=1.65)|], ()),
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

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0
    || value->int_of_string_opt->Belt.Option.isSome
      ? Valid : Error("not an int");
  fieldState;
};

[@react.component]
let make = (~onPressCancel) => {
  let account = StoreContext.useAccount();

  let (advancedOptionOpened, setAdvancedOptionOpened) =
    React.useState(_ => false);

  let (operationRequest, sendOperation) =
    OperationApiRequest.useCreateOperation();

  let form: SendForm.api =
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            nonEmpty(Amount)
            + custom(values => isValidFloat(values.amount), Amount)
            + nonEmpty(Sender)
            + nonEmpty(Recipient)
            + custom(values => isValidFloat(values.fee), Fee)
            + custom(values => isValidInt(values.counter), Counter)
            + custom(values => isValidInt(values.gasLimit), GasLimit)
            + custom(values => isValidInt(values.storageLimit), StorageLimit),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let operation =
            Injection.Transaction({
              source: state.values.sender,
              amount: state.values.amount->Js.Float.fromString,
              destination: state.values.recipient,
              fee:
                advancedOptionOpened && state.values.fee->Js.String2.length > 0
                  ? Some(state.values.fee->Js.Float.fromString) : None,
              counter:
                advancedOptionOpened
                && state.values.counter->Js.String2.length > 0
                  ? Some(state.values.counter->int_of_string) : None,
              gasLimit:
                advancedOptionOpened
                && state.values.gasLimit->Js.String2.length > 0
                  ? Some(state.values.gasLimit->int_of_string) : None,
              storageLimit:
                advancedOptionOpened
                && state.values.storageLimit->Js.String2.length > 0
                  ? Some(state.values.storageLimit->int_of_string) : None,
              forceLowFee:
                advancedOptionOpened && state.values.forceLowFee
                  ? Some(true) : None,
              burnCap: None,
              confirmations: None,
            });

          sendOperation(operation);

          None;
        },
      ~initialState={
        amount: "",
        sender:
          account->Belt.Option.mapWithDefault("", account => account.address),
        recipient: "",
        fee: "",
        counter: "",
        gasLimit: "",
        storageLimit: "",
        forceLowFee: false,
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView>
    {switch (operationRequest) {
     | Done(Ok(hash)) =>
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
     | Done(Error(error)) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Loading =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color=Colors.highIcon
         />
       </View>
     | NotAsked =>
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
             onPress={_ => setAdvancedOptionOpened(prev => !prev)}>
             <Typography.Overline1>
               "Advanced options"->React.string
             </Typography.Overline1>
             <Text
               style=Style.(
                 arrayOption([|
                   Some(styles##chevron),
                   advancedOptionOpened ? Some(styles##chevronOpened) : None,
                 |])
               )>
               {js|∨|js}->React.string
             </Text>
           </TouchableOpacity>
           {advancedOptionOpened
              ? <SendViewAdvancedOptions form /> : React.null}
         </View>
         <View style=styles##formAction>
           <FormButton text="CANCEL" onPress=onPressCancel />
           <FormButton
             text="OK"
             onPress=onSubmit
             disabled={form.formState == Errored}
           />
         </View>
       </>
     }}
  </ModalView>;
};
