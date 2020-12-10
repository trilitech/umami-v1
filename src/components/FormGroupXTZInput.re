let formatOnBlur = (token, setValue) =>
  if (token->Belt.Option.isSome) {
    setValue(BusinessUtils.formatToken);
  } else {
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
      ~token: option(Token.t)=?,
    ) => {
  let (value, setValue) = React.useState(() => value);
  <FormGroupTextInput
    label
    ?style
    value
    error
    onBlur={_ => formatOnBlur(token, setValue)}
    decoration={decoration->Belt.Option.getWithDefault(xtzDecoration)}
    handleChange={text => {
      handleChange(text);
      setValue(_ => text);
    }}
    keyboardType=`numeric
  />;
};
