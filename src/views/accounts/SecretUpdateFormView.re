module StateLenses = [%lenses type state = {name: string}];
module SecretCreateForm = ReForm.Make(StateLenses);

[@react.component]
let make = (~secret: Secret.t, ~closeAction) => {
  let (updateSecretRequest, updateSecret) = StoreContext.Secrets.useUpdate();

  let addLog = LogsContext.useAdd();

  let action = secret => {
    updateSecret(secret)
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#secret_updated
      );
  };

  let form: SecretCreateForm.api =
    SecretCreateForm.use(
      ~schema={
        SecretCreateForm.Validation.(Schema(nonEmpty(Name)));
      },
      ~onSubmit=
        ({state}) => {
          let {index, derivationScheme, addresses, legacyAddress} = secret;
          action(
            Secret.{
              name: state.values.name,
              index,
              derivationScheme,
              addresses,
              legacyAddress,
            },
          )
          ->Future.tapOk(() => closeAction())
          ->ignore;

          None;
        },
      ~initialState={name: secret.name},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = updateSecretRequest->ApiRequest.isLoading;

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      I18n.title#secret_update->React.string
    </Typography.Headline>
    <FormGroupTextInput
      label=I18n.label#account_create_name
      value={form.values.name}
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
    />
    <Buttons.SubmitPrimary
      text=I18n.btn#update
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
