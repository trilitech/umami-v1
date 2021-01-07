module StateLenses = [%lenses type state = {name: string}];
module AccountCreateForm = ReForm.Make(StateLenses);

[@react.component]
let make = (~closeAction) => {
  let (accountRequest, createAccount) = StoreContext.Accounts.useCreate();
  let addLog = LogsContext.useAdd();

  let form: AccountCreateForm.api =
    AccountCreateForm.use(
      ~schema={
        AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
      },
      ~onSubmit=
        ({state}) => {
          createAccount(state.values.name)
          ->Future.tapOk(_ => closeAction())
          ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
              I18n.t#account_created
            )
          ->ignore;
          None;
        },
      ~initialState={name: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = accountRequest != ApiRequest.NotAsked;

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      I18n.title#account_create->React.string
    </Typography.Headline>
    <FormGroupTextInput
      label=I18n.label#account_create_name
      value={form.values.name}
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
    />
    <Buttons.SubmitPrimary
      text=I18n.btn#create
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
    />
  </ModalFormView>;
};
