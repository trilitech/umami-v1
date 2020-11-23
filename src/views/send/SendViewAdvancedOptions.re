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
let make = (~form: SendForm.api) => {
  let network = StoreContext.useNetwork();
  let (operationSimulateRequest, sendOperationSimulate) =
    OperationApiRequest.useSimulate(network);

  React.useEffect0(() => {
    if (form.values.sender != ""
        && form.values.recipient != ""
        && form.values.amount != "") {
      let operation =
        Injection.makeTransfer(
          ~source=form.values.sender,
          ~amount=form.values.amount->Js.Float.fromString,
          ~destination=form.values.sender,
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
          form.handleChange(GasLimit, dryRun.gasLimit->string_of_int);
          form.handleChange(StorageLimit, dryRun.storageLimit->string_of_int);
          form.handleChange(Counter, dryRun.count->string_of_int);
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
        label="Gas limit"
        value={form.values.gasLimit}
        handleChange={form.handleChange(GasLimit)}
        error={form.getFieldError(Field(GasLimit))}
        style=styles##formRowInput
      />
      <View style=styles##formRowInputsSeparator />
      <FormGroupTextInput
        label="Storage limit"
        value={form.values.storageLimit}
        handleChange={form.handleChange(StorageLimit)}
        error={form.getFieldError(Field(StorageLimit))}
        style=styles##formRowInput
      />
    </View>
    <View style=styles##formRowInputs>
      <FormGroupTextInput
        label="Counter"
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
