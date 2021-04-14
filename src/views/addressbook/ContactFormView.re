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

let addressExistsCheck =
    (aliases, values: StateLenses.state): ReSchema.fieldState => {
  switch (aliases->Map.String.get(values.address)) {
  | None => Valid
  | Some(a: Account.t) =>
    Error(I18n.form_input_error#key_already_registered(a.alias))
  };
};

let formCheckExists = (aliases, values: StateLenses.state) =>
  AliasHelpers.formCheckExists(aliases, values.name);

[@react.component]
let make = (~initAddress=?, ~action: action, ~closeAction) => {
  let (createAliasRequest, createAlias) = StoreContext.Aliases.useCreate();
  let (updateAliasRequest, updateAlias) = StoreContext.Aliases.useUpdate();

  let addToast = LogsContext.useToast();

  let aliasesRequest = StoreContext.Aliases.useRequest();
  let aliases =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault(Map.String.empty);

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(
          Schema(
            nonEmpty(Name)
            + nonEmpty(Address)
            + custom(addressExistsCheck(aliases), Address)
            + custom(formCheckExists(aliases), Name),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          switch (action) {
          | Create =>
            createAlias((state.values.name, state.values.address))
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addToast, Logs.Account, _ =>
                I18n.t#contact_added
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
        | Create => {
            name: "",
            address: initAddress->Option.getWithDefault(""),
          }
        | Edit(account) => {name: account.alias, address: account.address}
        },
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading =
    createAliasRequest->ApiRequest.isLoading
    || updateAliasRequest->ApiRequest.isLoading;

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

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
      placeholder=I18n.input_placeholder#add_contacts_name
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
    />
    <FormGroupTextInput
      label=I18n.label#add_contact_address
      value={form.values.address}
      placeholder=I18n.input_placeholder#add_contacts_tz
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
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
