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

let passwordLengthCheck = (values: StateLenses.state) => {
  let fieldState: ReSchema.fieldState =
    values.password->Js.String.length >= 8
      ? Valid : Error(I18n.form_input_error#password_length);
  fieldState;
};

[@react.component]
let make =
    (
      ~mnemonic,
      ~onPressCancel,
      ~createSecretWithMnemonic,
      ~loading,
      ~existingSecretsCount=0,
    ) => {
  let displayConfirmPassword = existingSecretsCount < 1;
  let form: CreatePasswordForm.api =
    CreatePasswordForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        CreatePasswordForm.Validation.(
          Schema(
            nonEmpty(Password)
            + (
              displayConfirmPassword
                ? custom(passwordLengthCheck, Password) : [||]
            )
            + (
              displayConfirmPassword
                ? custom(isConfirmPassword, ConfirmPassword) : [||]
            ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          let mnemonics = mnemonic->Js.Array2.joinWith(" ");
          createSecretWithMnemonic(
            SecretApiRequest.{
              name: "Secret " ++ (existingSecretsCount + 1)->string_of_int,
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

  let passwordPlaceholder =
    displayConfirmPassword
      ? I18n.input_placeholder#enter_new_password
      : I18n.input_placeholder#enter_password;

  <>
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      placeholder=passwordPlaceholder
      textContentType=`password
      secureTextEntry=true
    />
    {displayConfirmPassword
       ? <FormGroupTextInput
           label=I18n.label#confirm_password
           value={form.values.confirmPassword}
           handleChange={form.handleChange(ConfirmPassword)}
           error={form.getFieldError(Field(ConfirmPassword))}
           placeholder=I18n.input_placeholder#confirm_password
           textContentType=`password
           secureTextEntry=true
         />
       : React.null}
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
