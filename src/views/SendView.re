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
  }
];
module SendForm = ReForm.Make(StateLenses);

module Modal = {
  let styles =
    Style.(
      StyleSheet.create({
        "modal":
          style(
            ~width=642.->dp,
            ~alignSelf=`center,
            ~paddingTop=45.->dp,
            ~paddingBottom=32.->dp,
            ~paddingHorizontal=110.->dp,
            ~backgroundColor="#121212",
            ~borderRadius=4.,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children) => {
    <View style=styles##modal> children </View>;
  };
};

module FormGroupTextInput = {
  let styles =
    Style.(
      StyleSheet.create({
        "formGroup": style(~flex=1., ~marginVertical=10.->dp, ()),
        "formGroupSmall": style(~marginVertical=7.->dp, ()),
        "label":
          style(
            ~marginBottom=6.->dp,
            ~color="rgba(255,255,255,0.8)",
            ~fontSize=18.,
            ~fontWeight=`_400,
            (),
          ),
        "labelError": style(~color="#f97977", ()),
        "labelSmall": style(~marginBottom=4.->dp, ~fontSize=16., ()),
        "input":
          style(
            ~height=46.->dp,
            ~paddingVertical=10.->dp,
            ~paddingLeft=20.->dp,
            ~paddingRight=12.->dp,
            ~color="rgba(255,255,255,0.8)",
            ~fontSize=16.,
            ~fontWeight=`_400,
            ~borderColor="rgba(255,255,255,0.6)",
            ~borderWidth=1.,
            ~borderRadius=5.,
            (),
          ),
        "inputError": style(~color="#f97977", ~borderColor="#f97977", ()),
        "inputSmall": style(~height=44.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (~label, ~value, ~handleChange, ~error, ~keyboardType=?, ~small=false) => {
    let hasError = error->Belt.Option.isSome;
    <View
      style=Style.(
        arrayOption([|
          Some(styles##formGroup),
          small ? Some(styles##formGroupSmall) : None,
        |])
      )>
      <Text
        style=Style.(
          arrayOption([|
            Some(styles##label),
            hasError ? Some(styles##labelError) : None,
            small ? Some(styles##labelSmall) : None,
          |])
        )>
        label->React.string
      </Text>
      <TextInput
        style=Style.(
          arrayOption([|
            Some(styles##input),
            hasError ? Some(styles##inputError) : None,
            small ? Some(styles##inputSmall) : None,
          |])
        )
        value
        onChange={(event: TextInput.changeEvent) =>
          handleChange(event.nativeEvent.text)
        }
        autoCapitalize=`none
        autoCorrect=false
        autoFocus=false
        ?keyboardType
      />
    </View>;
  };
};

module FormButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~width=160.->dp,
            ~height=46.->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            (),
          ),
        "text":
          style(
            ~color="rgba(255,255,255,0.87)",
            ~fontSize=14.,
            ~fontWeight=`_600,
            (),
          ),
        "textDisabled":
          style(
            ~color="rgba(255,255,255,0.38)",
            ~fontSize=14.,
            ~fontWeight=`_600,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~text, ~onPress, ~disabled=?) => {
    <TouchableOpacity style=styles##button onPress ?disabled>
      <Text
        style=Style.(
          arrayOption([|
            Some(styles##text),
            disabled->Belt.Option.flatMap(disabled =>
              disabled ? Some(styles##textDisabled) : None
            ),
          |])
        )>
        text->React.string
      </Text>
    </TouchableOpacity>;
  };
};

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

let isValidNumber = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN
      ? Error("not a number") : Valid;
  fieldState;
};

[@react.component]
let make = () => {
  let (account, _) = React.useContext(Account.context);
  let (network, _) = React.useContext(Network.context);

  let (operationDone, setOperationDone) = React.useState(_ => false);

  let (_href, onPressCancel) = Routes.useHrefAndOnPress(Routes.Home);

  let form: SendForm.api =
    SendForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        SendForm.Validation.(
          Schema(
            custom(values => isValidNumber(values.amount), Amount)
            + nonEmpty(Sender)
            + nonEmpty(Recipient)
            + custom(values => isValidNumber(values.fee), Fee)
            + custom(values => isValidNumber(values.counter), Counter)
            + custom(values => isValidNumber(values.gasLimit), GasLimit)
            + custom(
                values => isValidNumber(values.storageLimit),
                StorageLimit,
              )
            + custom(values => isValidNumber(values.burnCap), BurnCap)
            + custom(
                values => isValidNumber(values.confirmations),
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
              fee: None,
              counter: None,
              gasLimit: None,
              storageLimit: None,
              burnCap: None,
              confirmations: None,
              forceLowFee: None,
            });

          network
          ->OperationsAPI.create(operation)
          ->Future.get(result =>
              switch (result) {
              | Ok(hash) => setOperationDone(_ => true)
              | Error(value) => Dialog.error(value)
              }
            );

          None;
        },
      ~initialState={
        amount: "1.0",
        sender: account,
        recipient: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
        fee: "",
        counter: "",
        gasLimit: "",
        storageLimit: "",
        burnCap: "",
        confirmations: "",
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <View>
    <Modal>
      {operationDone
         ? <>
             <Text style=styles##title>
               "Operation injected in the node"->React.string
             </Text>
             <Text style=FormGroupTextInput.styles##label>
               "Operation hash"->React.string
             </Text>
             <Text style=styles##hash>
               "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
               ->React.string
             </Text>
             <View style=styles##formAction>
               <FormButton text="OK" onPress=onPressCancel />
             </View>
           </>
         : <>
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
             <View style=styles##formAction>
               <FormButton text="CANCEL" onPress=onPressCancel />
               <FormButton
                 text="OK"
                 onPress=onSubmit
                 disabled={form.formState == Errored}
               />
             </View>
           </>}
    </Modal>
  </View>;
};
