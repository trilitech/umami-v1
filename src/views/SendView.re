open ReactNative;

module StateLenses = [%lenses
  type state = {
    amount: float,
    sender: string,
    recipient: string,
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
        "formGroup": style(~marginVertical=10.->dp, ()),
        "label":
          style(
            ~marginBottom=6.->dp,
            ~color="rgba(255,255,255,0.8)",
            ~fontSize=18.,
            ~fontWeight=`_400,
            (),
          ),
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
      })
    );

  [@react.component]
  let make = (~label, ~value, ~handleChange, ~keyboardType=?) => {
    <View style=styles##formGroup>
      <Text style=styles##label> label->React.string </Text>
      <TextInput
        style=styles##input
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
             <View style=styles##formAction>
               <FormButton text="CANCEL" onPress=onPressCancel />
               <FormButton text="OK" onPress=onSubmit />
             </View>
           </>}
    </Modal>
  </View>;
};
