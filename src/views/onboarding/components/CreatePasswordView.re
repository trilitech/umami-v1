open ReactNative;

module StateLenses = [%lenses
  type state = {
    password: string,
    confirmPassword: string,
  }
];

module CreatePasswordForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "wordsList": style(~flexDirection=`row, ~flexWrap=`wrap, ()),
      "wordItem":
        style(
          ~marginVertical=2.->dp,
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=40.->pct,
          (),
        ),
      "wordSpacer": style(~width=20.->dp, ()),
    })
  );

let isConfirmPassword = (values: StateLenses.state) => {
  let fieldState: ReSchema.fieldState =
    values.confirmPassword == values.password
      ? Valid : Error(I18n.form_input_error#confirm_password);
  fieldState;
};

[@react.component]
let make = (~mnemonic, ~onPressCancel, ~createSecretWithMnemonic, ~loading) => {
  let form: CreatePasswordForm.api =
    CreatePasswordForm.use(
      ~schema={
        CreatePasswordForm.Validation.(
          Schema(
            nonEmpty(Password) + custom(isConfirmPassword, ConfirmPassword),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let mnemonics = mnemonic->Js.Array2.joinWith(" ");
          createSecretWithMnemonic(
            SecretApiRequest.{
              name: "Account 1",
              mnemonics,
              password: state.values.password,
            },
          );

          None;
        },
      ~initialState={password: "", confirmPassword: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

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
    />
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.confirmPassword}
      handleChange={form.handleChange(ConfirmPassword)}
      error={form.getFieldError(Field(ConfirmPassword))}
      textContentType=`password
      secureTextEntry=true
    />
    <View style=FormStyles.formActionSpaceBetween>
      <Buttons.Form
        text=I18n.btn#back
        onPress=onPressCancel
        disabled=loading
      />
      <Buttons.SubmitPrimary
        text=I18n.btn#finish
        onPress=onSubmit
        loading
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
