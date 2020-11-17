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
      "formAction":
        style(
          ~marginTop=28.->dp,
          ~flexDirection=`row,
          ~justifyContent=`center,
          (),
        ),
    })
  );

let isConfirmPassword = (values: StateLenses.state) => {
  let fieldState: ReSchema.fieldState =
    values.confirmPassword == values.password
      ? Valid : Error("not the same password");
  fieldState;
};

[@react.component]
let make = (~mnemonic, ~onPressCancel, ~createAccountWithMnemonic) => {
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
          createAccountWithMnemonic(
            AccountApiRequest.{
              name: "Account 1",
              mnemonics,
              password: state.values.password,
            },
          );

          None;
        },
      ~initialState={password: "", confirmPassword: ""},
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  <>
    <FormGroupTextInput
      label="Password"
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      textContentType=`password
      secureTextEntry=true
    />
    <FormGroupTextInput
      label="Confirm password"
      value={form.values.confirmPassword}
      handleChange={form.handleChange(ConfirmPassword)}
      error={form.getFieldError(Field(ConfirmPassword))}
      textContentType=`password
      secureTextEntry=true
    />
    <View style=styles##formAction>
      <FormButton text="CANCEL" onPress=onPressCancel />
      <FormButton text="FINISH" onPress=onSubmit />
    </View>
  </>;
};
