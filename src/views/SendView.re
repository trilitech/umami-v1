open ReactNative;

module StateLenses = [%lenses
  type state = {
    amount: float,
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
        "inputSmall": style(~height=44.->dp, ()),
      })
    );

  [@react.component]
  let make = (~label, ~value, ~handleChange, ~keyboardType=?, ~small=false) => {
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
            small ? Some(styles##labelSmall) : None,
          |])
        )>
        label->React.string
      </Text>
      <TextInput
        style=Style.(
          arrayOption([|
            Some(styles##input),
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
      })
    );

  [@react.component]
  let make = (~text, ~onPress) => {
    <TouchableOpacity style=styles##button onPress>
      <Text style=styles##text> text->React.string </Text>
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

module Operations = API.Operations(API.TezosExplorer);

[@react.component]
let make = () => {
  let (account, _) = React.useContext(Account.context);
  let (network, _) = React.useContext(Network.context);

  let (operationDone, setOperationDone) = React.useState(_ => false);

  let (_href, onPressCancel) = Routes.useHrefAndOnPress(Routes.Home);

  let form: SendForm.api =
    SendForm.use(
      ~schema={
        SendForm.Validation.(
          Schema(
            /*nonEmpty(Amount) +*/ nonEmpty(Sender) + nonEmpty(Recipient),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let operation =
            Injection.Transaction({
              source: state.values.sender,
              amount: state.values.amount,
              destination: state.values.recipient,
            });

          network
          ->Operations.create(operation)
          ->Future.get(result =>
              switch (result) {
              | Ok(hash) => setOperationDone(_ => true)
              | Error(value) => Dialog.error(value)
              }
            );

          None;
        },
      ~initialState={
        amount: 1.0,
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
               value={form.values.amount->Js.Float.toString}
               handleChange={amountValue =>
                 amountValue
                 ->Js.Float.fromString
                 ->(x => x->Js.Float.isNaN ? 0.0 : x)
                 |> form.handleChange(Amount)
               }
               keyboardType=`numeric
             />
             <FormGroupTextInput
               label="Sender account"
               value={form.values.sender}
               handleChange={form.handleChange(Sender)}
             />
             <FormGroupTextInput
               label="Recipient account"
               value={form.values.recipient}
               handleChange={form.handleChange(Recipient)}
             />
             <View style=styles##formRowInputs>
               <FormGroupTextInput
                 label="Fee"
                 value={form.values.fee}
                 handleChange={form.handleChange(Fee)}
                 small=true
               />
               <View style=styles##formRowInputsSeparator />
               <FormGroupTextInput
                 label="Counter"
                 value={form.values.counter}
                 handleChange={form.handleChange(Counter)}
                 small=true
               />
               <View style=styles##formRowInputsSeparator />
               <FormGroupTextInput
                 label="Gas limit"
                 value={form.values.gasLimit}
                 handleChange={form.handleChange(GasLimit)}
                 small=true
               />
             </View>
             <View style=styles##formRowInputs>
               <FormGroupTextInput
                 label="Storage limit"
                 value={form.values.storageLimit}
                 handleChange={form.handleChange(StorageLimit)}
                 small=true
               />
               <View style=styles##formRowInputsSeparator />
               <FormGroupTextInput
                 label="Burn cap"
                 value={form.values.burnCap}
                 handleChange={form.handleChange(BurnCap)}
                 small=true
               />
               <View style=styles##formRowInputsSeparator />
               <FormGroupTextInput
                 label="Confirmations"
                 value={form.values.confirmations}
                 handleChange={form.handleChange(Confirmations)}
                 small=true
               />
             </View>
             <View style=styles##formAction>
               <FormButton text="CANCEL" onPress=onPressCancel />
               <FormButton text="OK" onPress=onSubmit />
             </View>
           </>}
    </Modal>
  </View>;
};
