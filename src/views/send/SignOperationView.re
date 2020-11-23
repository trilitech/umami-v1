open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "formAction":
        style(
          ~flexDirection=`row,
          ~justifyContent=`center,
          ~marginTop=24.->dp,
          (),
        ),
      "operationSummary": style(~marginBottom=20.->dp, ()),
    })
  );

[@react.component]
let make = (~onPressCancel, ~operation: Injection.operation, ~sendOperation) => {
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
    <View style=styles##title>
      <Typography.Headline2>
        (
          switch (operation) {
          | Transaction({amount}) =>
            I18n.t#xtz_amount(
              Js.Float.toFixedWithPrecision(amount, ~digits=1),
            )
          | Delegation(_) => I18n.title#delegate
          }
        )
        ->React.string
      </Typography.Headline2>
      {switch (operation) {
       | Transaction({fee})
       | Delegation({fee}) =>
         fee->ReactUtils.mapOpt(fee =>
           <Typography.Body1 colorStyle=`mediumEmphasis>
             {I18n.t#operation_summary_fee(fee->Js.Float.toString)
              ->React.string}
           </Typography.Body1>
         )
       }}
    </View>
    <OperationSummaryView style=styles##operationSummary operation />
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      textContentType=`password
      secureTextEntry=true
    />
    <View style=styles##formAction>
      <FormButton text=I18n.btn#cancel onPress=onPressCancel />
      <FormButton text=I18n.btn#confirm onPress={operation->onSubmit} />
    </View>
  </>;
};
