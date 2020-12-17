let formatOnBlur = (token, setValue) =>
  if (token->Belt.Option.isSome) {
    setValue(BusinessUtils.formatToken);
  } else {
    setValue(BusinessUtils.formatXTZ);
  };

let xtzDecoration = (~style) =>
  <Typography.Body1 colorStyle=`mediumEmphasis style>
    BusinessUtils.xtz->React.string
  </Typography.Body1>;

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

  // reformat value if token change
  React.useEffect1(
    () => {
      if (value != "") {
        formatOnBlur(token, setValue);
      };
      None;
    },
    [|token|],
  );

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
