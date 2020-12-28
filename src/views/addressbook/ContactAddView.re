open ReactNative;

module StateLenses = [%lenses
  type state = {
    name: string,
    address: string,
  }
];
module AccountCreateForm = ReForm.Make(StateLenses);

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
         <Typography.Headline style=FormStyles.header>
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
         <View style=FormStyles.formAction>
           <Buttons.FormPrimary text=I18n.btn#cancel onPress=onPressCancel />
           <Buttons.FormPrimary text=I18n.btn#add onPress=onSubmit />
         </View>
       </>
     }}
  </ModalView.Form>;
};
