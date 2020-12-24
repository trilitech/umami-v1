open ReactNative;

let styles =
  Style.(
    StyleSheet.create({"operationSummary": style(~marginBottom=20.->dp, ())})
  );

[@react.component]
let make =
    (
      ~onPressCancel,
      ~operation,
      ~title,
      ~fee,
      ~source,
      ~destination,
      ~sendOperation,
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

  let onSubmit = (operation, _) => {
    // checking password
    // getting stored data
    form.submit();
    sendOperation(operation, ~password=form.values.password);
  };

  <>
    <View style=FormStyles.title>
      <Typography.Headline> title->React.string </Typography.Headline>
      {fee->ReactUtils.mapOpt(fee =>
         <Typography.Body1 colorStyle=`mediumEmphasis>
           {I18n.t#operation_summary_fee(fee->Js.Float.toString)->React.string}
         </Typography.Body1>
       )}
    </View>
    <OperationSummaryView style=styles##operationSummary source destination />
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
      <Buttons.FormPrimary
        text=I18n.btn#confirm
        onPress={operation->onSubmit}
      />
    </View>
  </>;
};
