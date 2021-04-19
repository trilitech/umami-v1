open ReactNative;

let styles =
  Style.(
    StyleSheet.create({"operationSummary": style(~marginBottom=20.->dp, ())})
  );

[@react.component]
let make =
    (
      ~title,
      ~subtitle=?,
      ~source,
      ~destinations,
      ~showCurrency,
      ~sendOperation,
      ~content,
      ~loading=false,
    ) => {
  let form: SendForm.Password.api =
    SendForm.Password.use(
      ~schema={
        SendForm.Password.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=
        ({state}) => {
          sendOperation(state.values.password);

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
    <View style=FormStyles.header>
      <Typography.Headline> title->React.string </Typography.Headline>
      {subtitle->ReactUtils.mapOpt(subtitle =>
         <Typography.Overline1 style=FormStyles.subtitle>
           subtitle->React.string
         </Typography.Overline1>
       )}
    </View>
    <OperationSummaryView
      style=styles##operationSummary
      source
      destinations
      showCurrency
      content
    />
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      textContentType=`password
      secureTextEntry=true
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
