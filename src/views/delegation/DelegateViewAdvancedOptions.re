open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formRowInputs":
        style(~flexDirection=`row, ~justifyContent=`center, ()),
      "formRowInputsSeparator": style(~width=20.->dp, ()),
      "formRowInput":
        style(
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=0.->dp,
          ~marginVertical=5.->dp,
          (),
        ),
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
    OperationApiRequest.useSimulateOperation(network);

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
          /*form.handleChange(BurnCap, dryRun.burnCap->Js.Float.toString);*/
          ();
        })
      ->ignore;
      None;
    },
    [|operationSimulateRequest|],
  );

  <View>
    <View style=styles##formRowInputs>
      <FormGroupTextInput
        label="Fee"
        value={form.values.fee}
        handleChange={form.handleChange(Fee)}
        error={form.getFieldError(Field(Fee))}
        style=styles##formRowInput
        decoration=FormGroupXTZInput.xtzDecoration
      />
      <View style=styles##formRowInputsSeparator />
      <FormGroupTextInput
        label="Burn cap"
        value={form.values.burnCap}
        handleChange={form.handleChange(BurnCap)}
        error={form.getFieldError(Field(BurnCap))}
        style=styles##formRowInput
        decoration=FormGroupXTZInput.xtzDecoration
      />
    </View>
    <FormGroupCheckbox
      label="Force low fee"
      value={form.values.forceLowFee}
      handleChange={form.handleChange(ForceLowFee)}
      error={form.getFieldError(Field(ForceLowFee))}
    />
    {operationSimulateRequest->ApiRequest.isLoading
       ? <View style=styles##loadingOverlay> <LoadingView /> </View>
       : React.null}
  </View>;
};
