open ReactNative;

module PasswordView = {
  module StateLenses = [%lenses type state = {password: string}];

  module PasswordForm = ReForm.Make(StateLenses);

  [@react.component]
  let make = (~recoveryPhraseRequest, ~getRecoveryPhrase) => {
    let form: PasswordForm.api =
      PasswordForm.use(
        ~schema={
          PasswordForm.Validation.(Schema(nonEmpty(Password)));
        },
        ~onSubmit=
          ({state}) => {
            getRecoveryPhrase(state.values.password);
            None;
          },
        ~initialState={password: ""},
        ~i18n=FormUtils.i18n,
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    let loading = recoveryPhraseRequest->ApiRequest.isLoading;

    let formFieldsAreValids =
      FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

    <>
      <FormGroupTextInput
        label=I18n.label#password
        value={form.values.password}
        handleChange={form.handleChange(Password)}
        error={form.getFieldError(Field(Password))}
        textContentType=`password
        secureTextEntry=true
        onSubmitEditing={_event => {form.submit()}}
      />
      <View style=FormStyles.verticalFormAction>
        <Buttons.SubmitPrimary
          text=I18n.btn#confirm
          onPress=onSubmit
          loading
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </>;
  };
};

module RecoveryPhrase = {
  [@react.component]
  let make = (~recoveryPhrase: string) => {
    let mnemonic =
      React.useRef(Bip39.generate(256)->Js.String2.split(" ")).current;

    Js.log2("Todo : use the recoveryPhrase", recoveryPhrase);

    <MnemonicListView mnemonic />;
  };
};

[@react.component]
let make = (~secret: Secret.t, ~closeAction) => {
  let (recoveryPhraseRequest, getRecoveryPhrase) =
    StoreContext.Secrets.useGetRecoveryPhrase(~index=secret.index);

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      I18n.title#secret_recovery->React.string
    </Typography.Headline>
    {switch (recoveryPhraseRequest) {
     | Done(Ok(recoveryPhrase), _) => <RecoveryPhrase recoveryPhrase />
     | _ => <PasswordView recoveryPhraseRequest getRecoveryPhrase />
     }}
  </ModalFormView>;
};
