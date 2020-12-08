let formatOnBlur = setValue => {
  setValue(BusinessUtils.formatXTZ);
};

let xtzDecoration = (~style) =>
  <Typography.Body1 style> BusinessUtils.xtz->React.string </Typography.Body1>;

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~handleChange,
      ~error,
      ~style: option(ReactNative.Style.t)=?,
      ~decoration=?,
    ) => {
  let (value, setValue) = React.useState(() => value);
  <FormGroupTextInput
    label
    ?style
    value
    error
    onBlur={_ => formatOnBlur(setValue)}
    decoration={decoration->Belt.Option.getWithDefault(xtzDecoration)}
    handleChange={text => {
      handleChange(text);
      setValue(_ => text);
    }}
    keyboardType=`numeric
  />;
};
