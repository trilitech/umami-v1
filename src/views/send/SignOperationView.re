open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=20.->dp, ~textAlign=`center, ()),
      "operationSummary": style(~marginBottom=20.->dp, ()),
    })
  );

[@react.component]
let make =
    (~title=?, ~onPressCancel, ~operation: SendForm.operation, ~sendOperation) => {
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
      <Typography.Headline>
        (
          switch (title, operation) {
          | (Some(title), _) => title
          | (_, InjectionOperation(Transaction({amount}))) =>
            I18n.t#xtz_amount(
              Js.Float.toFixedWithPrecision(amount, ~digits=1),
            )
          | (_, InjectionOperation(Delegation(_))) => I18n.title#delegate
          | (_, TokensOperation({action: Transfer({amount})}, token)) =>
            I18n.t#amount(amount->Js.Int.toString, token.symbol)
          | _ => ""
          }
        )
        ->React.string
      </Typography.Headline>
      {switch (operation) {
       | InjectionOperation(Transaction({options: {fee}}))
       | InjectionOperation(Delegation({fee}))
       | TokensOperation({options: {fee}}, _) =>
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
    <View style=FormStyles.formAction>
      <Buttons.FormPrimary text=I18n.btn#cancel onPress=onPressCancel />
      <Buttons.FormPrimary
        text=I18n.btn#confirm
        onPress={operation->onSubmit}
      />
    </View>
  </>;
};
