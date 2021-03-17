open ReactNative;

module StateLenses = [%lenses type state = {password: string}];

module PasswordForm = ReForm.Make(StateLenses);

[@react.component]
let make = (~loading=false, ~submitPassword) => {
  let form: PasswordForm.api =
    PasswordForm.use(
      ~schema={
        PasswordForm.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=
        ({state}) => {
          submitPassword(~password=state.values.password);
          None;
        },
      ~initialState={password: ""},
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
