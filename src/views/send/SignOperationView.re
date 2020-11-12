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
let make = (~onPressCancel, ~operation, ~sendOperation) => {
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
    sendOperation(Injection.Transaction(operation));
  };

  <>
    <View style=styles##title>
      <Typography.Headline2>
        {Js.Float.toFixedWithPrecision(operation.Injection.amount, ~digits=1)
         ->React.string}
        " XTZ"->React.string
      </Typography.Headline2>
      {operation.Injection.fee
       ->ReactUtils.mapOpt(fee =>
           <Typography.Body1 colorStyle=`mediumEmphasis>
             "+ Fee "->React.string
             {fee->Js.Float.toString->React.string}
             " XTZ"->React.string
           </Typography.Body1>
         )}
    </View>
    <OperationSummaryView
      style=styles##operationSummary
      transaction=operation
    />
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
      <FormButton
        text="SEND"
        onPress={operation->onSubmit}
      />
    </View>
  </>;
};
