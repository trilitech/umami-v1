open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formRowInputs":
        style(~flexDirection=`row, ~justifyContent=`center, ()),
      "formRowInputsSeparator": style(~width=13.->dp, ()),
      "formRowInput":
        style(
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=0.->dp,
          ~marginVertical=5.->dp,
          (),
        ),
    })
  );

let xtzDecoration = (~style) =>
  <Typography.Body1 style> BusinessUtils.xtz->React.string </Typography.Body1>;

[@react.component]
let make = (~form: SendForm.api, ~token: option(Token.t)=?) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  React.useEffect0(() => {
    if (form.values.sender != ""
        && form.values.recipient != ""
        && form.values.amount != "") {
      let operation = {
        switch (token) {
        | Some(token) =>
          OperationApiRequest.Token(
            Token.makeTransfer(
              ~source=form.values.sender,
              ~amount=form.values.amount->Js.Float.fromString->int_of_float,
              ~destination=form.values.recipient,
              ~contract=token.address,
              (),
            ),
          )
        | None =>
          OperationApiRequest.Regular(
            Injection.makeTransfer(
              ~source=form.values.sender,
              ~amount=form.values.amount->Js.Float.fromString,
              ~destination=form.values.recipient,
              (),
            ),
          )
        };
      };
      sendOperationSimulate(operation)
      ->Future.tapOk(dryRun => {
          form.handleChange(Fee, dryRun.fee->Js.Float.toString);
          form.handleChange(GasLimit, dryRun.gasLimit->string_of_int);
          form.handleChange(StorageLimit, dryRun.storageLimit->string_of_int);
          form.handleChange(Counter, dryRun.count->string_of_int);
        })
      ->ignore;
    };

    None;
  });

  let theme = ThemeContext.useTheme();

  <View>
    <View style=styles##formRowInputs>
      <FormGroupTextInput
        label=I18n.label#fee
        value={form.values.fee}
        handleChange={form.handleChange(Fee)}
        error={form.getFieldError(Field(Fee))}
        style=styles##formRowInput
        decoration=FormGroupXTZInput.xtzDecoration
      />
      <View style=styles##formRowInputsSeparator />
      <FormGroupTextInput
        label=I18n.label#gas_limit
        value={form.values.gasLimit}
        handleChange={form.handleChange(GasLimit)}
        error={form.getFieldError(Field(GasLimit))}
        style=styles##formRowInput
      />
      <View style=styles##formRowInputsSeparator />
      <FormGroupTextInput
        label=I18n.label#storage_limit
        value={form.values.storageLimit}
        handleChange={form.handleChange(StorageLimit)}
        error={form.getFieldError(Field(StorageLimit))}
        style=styles##formRowInput
      />
    </View>
    <View style=styles##formRowInputs>
      <FormGroupTextInput
        label=I18n.label#counter
        value={form.values.counter}
        handleChange={form.handleChange(Counter)}
        error={form.getFieldError(Field(Counter))}
        style=styles##formRowInput
      />
      <View style=styles##formRowInputsSeparator />
      <View style=styles##formRowInput />
      <View style=styles##formRowInputsSeparator />
      <View style=styles##formRowInput />
    </View>
    <FormGroupCheckbox
      label=I18n.label#force_low_fee
      value={form.values.forceLowFee}
      handleChange={form.handleChange(ForceLowFee)}
      error={form.getFieldError(Field(ForceLowFee))}
    />
    {ReactUtils.onlyWhen(
       <View
         style=Style.(
           array([|
             StyleSheet.absoluteFillObject,
             style(
               ~backgroundColor=theme.colors.background,
               ~opacity=0.87,
               (),
             ),
           |])
         )>
         <LoadingView />
       </View>,
       operationSimulateRequest->ApiRequest.isLoading,
     )}
  </View>;
};
