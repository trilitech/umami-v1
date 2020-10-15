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
      "title":
        style(
          ~marginBottom=20.->dp,
          ~textAlign=`center,
          ~color="rgba(255,255,255,0.87)",
          ~fontSize=22.,
          ~fontWeight=`_500,
          (),
        ),
      "hash":
        style(
          ~marginBottom=10.->dp,
          ~color="rgba(255,255,255,0.87)",
          ~fontSize=16.,
          ~fontWeight=`_300,
          ~textDecorationLine=`underline,
          (),
        ),
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
    })
  );

module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

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
let make = () => {
  let (account, _) = React.useContext(Account.context);
  let (network, _) = React.useContext(Network.context);

  let (operationDone, setOperationDone) = React.useState(_ => None);

  let (_href, onPressCancel) = Routes.useHrefAndOnPress(Routes.Home);

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
                state.values.fee->Js.String2.length > 0
                  ? Some(state.values.fee->Js.Float.fromString) : None,
              counter:
                state.values.counter->Js.String2.length > 0
                  ? Some(state.values.counter->int_of_string) : None,
              gasLimit:
                state.values.gasLimit->Js.String2.length > 0
                  ? Some(state.values.gasLimit->int_of_string) : None,
              storageLimit:
                state.values.storageLimit->Js.String2.length > 0
                  ? Some(state.values.storageLimit->int_of_string) : None,
              burnCap:
                state.values.burnCap->Js.String2.length > 0
                  ? Some(state.values.burnCap->Js.Float.fromString) : None,
              confirmations:
                state.values.confirmations->Js.String2.length > 0
                  ? Some(state.values.confirmations->int_of_string) : None,
              forceLowFee: state.values.forceLowFee ? Some(true) : None,
            });

          network
          ->OperationsAPI.create(operation)
          ->Future.get(result =>
              switch (result) {
              | Ok(hash) => setOperationDone(_ => Some(hash))
              | Error(value) => Dialog.error(value)
              }
            );

          None;
        },
      ~initialState={
        amount: "",
        sender: account,
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

  <View>
    <ModalView>
      {switch (operationDone) {
       | Some(hash) =>
         <>
           <Text style=styles##title>
             "Operation injected in the node"->React.string
           </Text>
           <Text style=FormLabel.styles##label>
             "Operation hash"->React.string
           </Text>
           <Text style=styles##hash> hash->React.string </Text>
           <View style=styles##formAction>
             <FormButton text="OK" onPress=onPressCancel />
           </View>
         </>
       | None =>
         <>
           <Text style=styles##title> "Send"->React.string </Text>
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
    </ModalView>
  </View>;
};
