open ReactNative;

let styles =
  Style.(
    StyleSheet.create({"formRowInput": style(~marginVertical=5.->dp, ())})
  );

let xtzDecoration = (~style) =>
  <Typography.Body1 style> I18n.t#xtz->React.string </Typography.Body1>;

[@react.component]
let make = (~form: DelegateForm.api) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  React.useEffect0(() => {
    if (form.values.sender != "" && form.values.baker != "") {
      let operation =
        Protocol.makeDelegate(
          ~source=form.values.sender,
          ~delegate=Some(form.values.baker),
          (),
        );
      let operation = Operation.Simulation.delegation(operation);
      sendOperationSimulate(operation)
      ->Future.tapOk(dryRun => {
          form.handleChange(Fee, dryRun.fee->ProtocolXTZ.toString)
        })
      ->ignore;
    };

    None;
  });

  let theme = ThemeContext.useTheme();

  <View>
    <FormGroupTextInput
      label=I18n.label#fee
      value={form.values.fee}
      handleChange={form.handleChange(Fee)}
      error={form.getFieldError(Field(Fee))}
      style=styles##formRowInput
      decoration=FormGroupXTZInput.xtzDecoration
    />
    <FormGroupCheckbox
      label=I18n.label#force_low_fee
      value={form.values.forceLowFee}
      handleChange={form.handleChange(ForceLowFee)}
      error={form.getFieldError(Field(ForceLowFee))}
    />
    {operationSimulateRequest->ApiRequest.isLoading
       ? <View
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
         </View>
       : React.null}
  </View>;
};
