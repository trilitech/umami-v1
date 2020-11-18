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
let make = (~onPressCancel, ~handleAdd) => {
  let (aliasRequest, createAlias) = AliasApiRequest.useCreate();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(
          Schema(nonEmpty(Name) + nonEmpty(Address))
        );
      },
      ~onSubmit=
        ({state}) => {
          createAlias((state.values.name, state.values.address))
          ->Future.tapOk(_ => handleAdd())
          ->ignore;

          None;
        },
      ~initialState={name: "", address: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView.Form>
    {switch (aliasRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.Headline2 style=styles##title>
           I18n.t#contact_added->React.string
         </Typography.Headline2>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | Done(Error(error)) =>
       <>
         <Typography.Body1 colorStyle=`error>
           error->React.string
         </Typography.Body1>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
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
         <Typography.Headline2 style=styles##title>
           I18n.title#add_contact->React.string
         </Typography.Headline2>
         <FormGroupTextInput
           label=I18n.label#add_contact_name
           value={form.values.name}
           handleChange={form.handleChange(Name)}
           error={form.getFieldError(Field(Name))}
         />
         <FormGroupTextInput
           label=I18n.label#add_contact_address
           value={form.values.address}
           handleChange={form.handleChange(Address)}
           error={form.getFieldError(Field(Address))}
         />
         <View style=styles##formAction>
           <FormButton text=I18n.btn#cancel onPress=onPressCancel />
           <FormButton text=I18n.btn#add onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
