open ReactNative;

let styles =
  Style.(
    StyleSheet.create({"operationSummary": style(~marginBottom=20.->dp, ())})
  );

[@react.component]
let make =
    (
      ~onPressCancel,
      ~title,
      ~subtitle=?,
      ~source,
      ~destination,
      ~sendOperation,
      ~content,
    ) => {
  let form: SendForm.Password.api =
    SendForm.Password.use(
      ~schema={
        SendForm.Password.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=({state: _}) => {None},
      ~initialState={password: ""},
      (),
    );

  let onSubmit = _ => {
    // checking password
    // getting stored data
    form.submit();
    sendOperation(form.values.password);
  };

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
      destination
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
    <View style=FormStyles.formAction>
      <Buttons.FormPrimary text=I18n.btn#cancel onPress=onPressCancel />
      <Buttons.FormPrimary text=I18n.btn#confirm onPress=onSubmit />
    </View>
  </>;
};
