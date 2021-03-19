module StateLenses = [%lenses type state = {name: string}];
module AccountCreateForm = ReForm.Make(StateLenses);

type action =
  | Create
  | Edit(Account.t);

module Generic = {
  [@react.component]
  let make =
      (
        ~init,
        ~title,
        ~buttonText,
        ~action: string => Future.t(Result.t('a, 'b)),
        ~request,
        ~closeAction,
      ) => {
    let form: AccountCreateForm.api =
      AccountCreateForm.use(
        ~schema={
          AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
        },
        ~onSubmit=
          ({state}) => {
            action(state.values.name)
            ->Future.tapOk(() => closeAction())
            ->ignore;

            None;
          },
        ~initialState={name: init},
        ~i18n=FormUtils.i18n,
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    let loading = request->ApiRequest.isLoading;

    let formFieldsAreValids =
      FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

    <ModalFormView closing={ModalFormView.Close(closeAction)}>
      <Typography.Headline style=FormStyles.header>
        title->React.string
      </Typography.Headline>
      <FormGroupTextInput
        label=I18n.label#account_create_name
        value={form.values.name}
        handleChange={form.handleChange(Name)}
        error={form.getFieldError(Field(Name))}
      />
      <Buttons.SubmitPrimary
        text=buttonText
        onPress=onSubmit
        loading
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </ModalFormView>;
  };
};

module Update = {
  [@react.component]
  let make = (~closeAction, ~account: Account.t) => {
    let (updateAccountRequest, updateAccount) =
      StoreContext.Accounts.useUpdate();

    let addLog = LogsContext.useAdd();

    let action = new_name => {
      updateAccount({old_name: account.alias, new_name})
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_updated
        );
    };

    <Generic
      init={account.alias}
      buttonText=I18n.btn#update
      title=I18n.title#account_update
      request=updateAccountRequest
      action
      closeAction
    />;
  };
};

module Create = {
  [@react.component]
  let make = (~closeAction) => {
    let (createAccountRequest, createAccount) =
      StoreContext.Accounts.useCreate();

    let addLog = LogsContext.useAdd();

    let action = (name): Future.t(Belt.Result.t(unit, string)) => {
      createAccount(name)
      ->Future.mapOk(_ => ())
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_created
        );
    };

    <Generic
      init=""
      buttonText=I18n.btn#create
      title=I18n.title#account_create
      request=createAccountRequest
      action
      closeAction
    />;
  };
};
