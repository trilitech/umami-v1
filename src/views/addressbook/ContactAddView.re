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
let make = (~cancel) => {
  let (aliasRequest, createAlias) = StoreContext.Aliases.useCreate();
  let addToast = LogsContext.useToast();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(
          Schema(nonEmpty(Name) + nonEmpty(Address))
        );
      },
      ~onSubmit=
        ({state}) => {
          cancel();
          createAlias((state.values.name, state.values.address))
          ->ApiRequest.logOk(addToast, Logs.Account, _ =>
              I18n.t#account_created
            )
          ->ignore;

          None;
        },
      ~initialState={name: "", address: ""},
      (),
    );

  let onPressCancel = _ => cancel();

  let onSubmit = _ => {
    form.submit();
  };

  <ModalView.Form>
    {switch (aliasRequest) {
     | Loading(_)
     | Done(_) => <> </>
     | NotAsked =>
       <>
         <Typography.Headline style=styles##title>
           I18n.title#add_contact->React.string
         </Typography.Headline>
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
