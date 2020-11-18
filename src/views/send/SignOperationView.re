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
            Js.Float.toFixedWithPrecision(amount, ~digits=1)
            ++ " "
            ++ BusinessUtils.xtz
          | Delegation(_) => "Delegate"
          }
        )
        ->React.string
      </Typography.Headline2>
      {switch (operation) {
       | Transaction({fee})
       | Delegation({fee}) =>
         fee->ReactUtils.mapOpt(fee =>
           <Typography.Body1 colorStyle=`mediumEmphasis>
             {("+ Fee " ++ fee->Js.Float.toString ++ " " ++ BusinessUtils.xtz)
              ->React.string}
           </Typography.Body1>
         )
       }}
    </View>
    <OperationSummaryView style=styles##operationSummary operation />
    <FormGroupTextInput
      label="Password"
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      textContentType=`password
      secureTextEntry=true
    />
    <View style=styles##formAction>
      <FormButton text="CANCEL" onPress=onPressCancel />
      <FormButton text="CONFIRM" onPress={operation->onSubmit} />
    </View>
  </>;
};
