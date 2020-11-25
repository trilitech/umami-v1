open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formRowInput": style(~marginVertical=5.->dp, ()),
      "loadingOverlay":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(~backgroundColor="rgba(23,23,23,0.87)", ()),
        |]),
    })
  );

let xtzDecoration = (~style) =>
  <Typography.Body1 style> BusinessUtils.xtz->React.string </Typography.Body1>;

[@react.component]
let make = (~form: DelegateForm.api) => {
  let network = StoreContext.useNetwork();
  let (operationSimulateRequest, sendOperationSimulate) =
    OperationApiRequest.useSimulate(network);

  React.useEffect0(() => {
    if (form.values.sender != "" && form.values.baker != "") {
      let operation =
        Injection.makeDelegate(
          ~source=form.values.sender,
          ~delegate=form.values.baker,
          (),
        );

      sendOperationSimulate(operation)->ignore;
    };

    None;
  });

  React.useEffect1(
    () => {
      operationSimulateRequest
      ->ApiRequest.getDoneOk
      ->Belt.Option.map(dryRun => {
          form.handleChange(Fee, dryRun.fee->Js.Float.toString);
          ();
        })
      ->ignore;
      None;
    },
    [|operationSimulateRequest|],
  );

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
       ? <View style=styles##loadingOverlay> <LoadingView /> </View>
       : React.null}
  </View>;
};
