open ReactNative;

module StateLenses = [%lenses
  type state = {
    amount: string,
    sender: string,
    recipient: string,
    fee: string,
    counter: string,
    gasLimit: string,
    storageLimit: string,
    burnCap: string,
    confirmations: string,
    forceLowFee: bool,
  }
];
module SendForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "formRowInputs":
        style(~flexDirection=`row, ~justifyContent=`center, ()),
      "formRowInputsSeparator": style(~width=13.->dp, ()),
      "formGroupSwitchSeparator": style(~height=11.->dp, ()),
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
            + custom(
                values => isValidInt(values.storageLimit),
                StorageLimit,
              )
            + custom(values => isValidFloat(values.burnCap), BurnCap)
            + custom(
                values => isValidInt(values.confirmations),
                Confirmations,
              ),
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
              burnCap:
                advancedOptionOpened
                && state.values.burnCap->Js.String2.length > 0
                  ? Some(state.values.burnCap->Js.Float.fromString) : None,
              confirmations:
                advancedOptionOpened
                && state.values.confirmations->Js.String2.length > 0
                  ? Some(state.values.confirmations->int_of_string) : None,
              forceLowFee:
                advancedOptionOpened && state.values.forceLowFee
                  ? Some(true) : None,
            });

          sendOperation(operation);

          None;
        },
      ~initialState={
        amount: "",
        sender:
          account->Belt.Option.mapWithDefault("", account => account.address),
        recipient: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
        fee: "",
        counter: "",
        gasLimit: "",
        storageLimit: "",
        burnCap: "",
        confirmations: "",
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
         <Typography.H2 style=styles##title>
           "Operation injected in the node"->React.string
         </Typography.H2>
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
         <Typography.H2 style=styles##title>
           "Send"->React.string
         </Typography.H2>
         <FormGroupTextInput
           label="Amount"
           value={form.values.amount}
           handleChange={form.handleChange(Amount)}
           error={form.getFieldError(Field(Amount))}
           keyboardType=`numeric
         />
         <FormGroupTextInput
           label="Sender account"
           value={form.values.sender}
           handleChange={form.handleChange(Sender)}
           error={form.getFieldError(Field(Sender))}
         />
         <FormGroupTextInput
           label="Recipient account"
           value={form.values.recipient}
           handleChange={form.handleChange(Recipient)}
           error={form.getFieldError(Field(Recipient))}
         />
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
            ? <>
                <View style=styles##formRowInputs>
                  <FormGroupTextInput
                    label="Fee"
                    value={form.values.fee}
                    handleChange={form.handleChange(Fee)}
                    error={form.getFieldError(Field(Fee))}
                    small=true
                  />
                  <View style=styles##formRowInputsSeparator />
                  <FormGroupTextInput
                    label="Counter"
                    value={form.values.counter}
                    handleChange={form.handleChange(Counter)}
                    error={form.getFieldError(Field(Counter))}
                    small=true
                  />
                  <View style=styles##formRowInputsSeparator />
                  <FormGroupTextInput
                    label="Gas limit"
                    value={form.values.gasLimit}
                    handleChange={form.handleChange(GasLimit)}
                    error={form.getFieldError(Field(GasLimit))}
                    small=true
                  />
                </View>
                <View style=styles##formRowInputs>
                  <FormGroupTextInput
                    label="Storage limit"
                    value={form.values.storageLimit}
                    handleChange={form.handleChange(StorageLimit)}
                    error={form.getFieldError(Field(StorageLimit))}
                    small=true
                  />
                  <View style=styles##formRowInputsSeparator />
                  <FormGroupTextInput
                    label="Burn cap"
                    value={form.values.burnCap}
                    handleChange={form.handleChange(BurnCap)}
                    error={form.getFieldError(Field(BurnCap))}
                    small=true
                  />
                  <View style=styles##formRowInputsSeparator />
                  <FormGroupTextInput
                    label="Confirmations"
                    value={form.values.confirmations}
                    handleChange={form.handleChange(Confirmations)}
                    error={form.getFieldError(Field(Confirmations))}
                    small=true
                  />
                </View>
                <View style=styles##formGroupSwitchSeparator />
                <FormGroupSwitch
                  label="Force low free"
                  value={form.values.forceLowFee}
                  handleChange={form.handleChange(ForceLowFee)}
                  error={form.getFieldError(Field(ForceLowFee))}
                />
              </>
            : React.null}
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
