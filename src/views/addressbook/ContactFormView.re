module StateLenses = [%lenses
  type state = {
    name: string,
    address: string,
  }
];
module AccountCreateForm = ReForm.Make(StateLenses);

type action =
  | Create
  | Edit(Account.t);

[@react.component]
let make = (~action: action, ~closeAction) => {
  let (createAliasRequest, createAlias) = StoreContext.Aliases.useCreate();
  let (updateAliasRequest, updateAlias) = StoreContext.Aliases.useUpdate();

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
          switch (action) {
          | Create =>
            createAlias((state.values.name, state.values.address))
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addToast, Logs.Account, _ =>
                I18n.t#account_created
              )
            ->ignore
          | Edit(account) =>
            updateAlias({
              new_name: state.values.name,
              old_name: account.alias,
            })
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addToast, Logs.Account, _ =>
                I18n.t#account_updated
              )
            ->ignore
          };

          None;
        },
      ~initialState=
        switch (action) {
        | Create => {name: "", address: ""}
        | Edit(account) => {name: account.alias, address: account.address}
        },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading =
    createAliasRequest->ApiRequest.isLoading
    || updateAliasRequest->ApiRequest.isLoading;

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      (
        switch (action) {
        | Create => I18n.title#add_contact
        | Edit(_) => I18n.title#update_contact
        }
      )
      ->React.string
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
      disabled=?{
        switch (action) {
        | Create => None
        | Edit(_) => Some(true)
        }
      }
    />
    <Buttons.SubmitPrimary
      text={
        switch (action) {
        | Create => I18n.btn#add
        | Edit(_) => I18n.btn#update
        }
      }
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
    />
  </ModalFormView>;
};
