module StateLenses = [%lenses
  type state = {
    name: string,
    secret: string,
  }
];
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
        ~action:
           (~name: string, ~secretIndex: int) => Future.t(Result.t('a, 'b)),
        ~request,
        ~closeAction,
        ~secret: option(Secret.t)=?,
        ~hideSecretSelector=false,
      ) => {
    let form: AccountCreateForm.api =
      AccountCreateForm.use(
        ~schema={
          AccountCreateForm.Validation.(Schema(nonEmpty(Name)));
        },
        ~onSubmit=
          ({state}) => {
            action(
              ~name=state.values.name,
              ~secretIndex=state.values.secret->Js.Float.fromString->int_of_float
            )
            ->Future.tapOk(() => closeAction())
            ->ignore;

            None;
          },
        ~initialState={
          name: init,
          secret:
            secret->Option.mapWithDefault("", secret =>
              secret.index->string_of_int
            ),
        },
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
      {hideSecretSelector
         ? React.null
         : <FormGroupSecretSelector
             label=I18n.label#account_secret
             value={form.values.secret}
             handleChange={form.handleChange(Secret)}
             error={form.getFieldError(Field(Secret))}
             disabled={secret->Option.isSome}
           />}
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

    let action = (~name as new_name, ~secretIndex as _s) => {
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
      hideSecretSelector=true
    />;
  };
};

module Create = {
  [@react.component]
  let make = (~closeAction, ~secret: option(Secret.t)=?) => {
    let (createDeriveRequest, deriveAccount) =
      StoreContext.Accounts.useDerive();

    let addLog = LogsContext.useAdd();

    let action =
        (~name, ~secretIndex): Future.t(Belt.Result.t(unit, string)) => {
      deriveAccount({name, index: secretIndex, password: "azerty"})
      ->Future.mapOk(_ => ())
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_created
        );
    };

    <Generic
      init=""
      buttonText=I18n.btn#add
      title=I18n.title#account_create
      request=createDeriveRequest
      action
      closeAction
      ?secret
    />;
  };
};
