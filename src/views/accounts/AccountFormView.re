module StateLenses = [%lenses type state = {name: string}];
module AccountCreateForm = ReForm.Make(StateLenses);

type action =
  | Create
  | Edit(Account.t);

[@react.component]
let make = (~action: action, ~closeAction) => {
  let (createAccountRequest, createAccount) =
    StoreContext.Accounts.useCreate();
  let (updateAccountRequest, updateAccount) =
    StoreContext.Accounts.useUpdate();

  let addLog = LogsContext.useAdd();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
      },
      ~onSubmit=
        ({state}) => {
          switch (action) {
          | Create =>
            createAccount(state.values.name)
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
                I18n.t#account_created
              )
            ->ignore
          | Edit(account) =>
            updateAccount({
              alias: state.values.name,
              address: account.address,
            })
            ->Future.tapOk(_ => closeAction())
            ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
                I18n.t#account_updated
              )
            ->ignore
          };

          None;
        },
      ~initialState={
        name:
          switch (action) {
          | Create => ""
          | Edit(account) => account.alias
          },
      },
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading =
    createAccountRequest->ApiRequest.isLoading
    || updateAccountRequest->ApiRequest.isLoading;

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      (
        switch (action) {
        | Create => I18n.title#account_create
        | Edit(_) => I18n.title#account_update
        }
      )
      ->React.string
    </Typography.Headline>
    <FormGroupTextInput
      label=I18n.label#account_create_name
      value={form.values.name}
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
    />
    <Buttons.SubmitPrimary
      text={
        switch (action) {
          | Create => I18n.btn#create
          | Edit(_) => I18n.btn#update
          }
      }
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
    />
  </ModalFormView>;
};
