open ReactNative;

module StateLenses = [%lenses
  type state = {
    name: string,
    address: string,
  }
];
module AccountCreateForm = ReForm.Make(StateLenses);

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
      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~onPressCancel) => {
  let (aliasRequest, createAlias) = AliasApiRequest.useCreateAlias();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(
          Schema(nonEmpty(Name) + nonEmpty(Address))
        );
      },
      ~onSubmit=
        ({state}) => {
          createAlias(state.values.name, state.values.address);
          None;
        },
      ~initialState={name: "", address: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView>
    {switch (aliasRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.H2 style=styles##title>
           "Contact added"->React.string
         </Typography.H2>
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
           color="#FFF"
         />
       </View>
     | NotAsked =>
       <>
         <Typography.H2 style=styles##title>
           "Add contact"->React.string
         </Typography.H2>
         <FormGroupTextInput
           label="Name"
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <FormGroupTextInput
           label="Address"
           value={form.values.address}
           handleChange={form.handleChange(Address)}
           error={form.getFieldError(Field(Address))}
         />
         <View style=styles##formAction>
           <FormButton text="CANCEL" onPress=onPressCancel />
           <FormButton
             text="ADD"
             onPress=onSubmit
             disabled={form.formState == Errored}
           />
         </View>
       </>
     }}
  </ModalView>;
};
