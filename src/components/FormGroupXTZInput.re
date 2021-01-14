let formatOnBlur = (token, setValue) =>
  if (token->Belt.Option.isSome) {
    setValue(BusinessUtils.formatToken);
  } else {
    setValue(s =>
      s->ProtocolXTZ.formatString->Belt.Option.getWithDefault("")
    );
  };

let xtzDecoration = (~style) =>
  <Typography.Body1 colorStyle=`mediumEmphasis style>
    I18n.t#xtz->React.string
  </Typography.Body1>;

[@react.component]
let make =
    (
      ~label,
      ~value: string,
      ~handleChange,
      ~error,
      ~style: option(ReactNative.Style.t)=?,
      ~decoration=?,
      ~setValue: option((string => string) => unit)=?,
      ~token: option(Token.t)=?,
    ) => {
  let innerValue = React.useState(() => value);

  let (value, setValue) =
    switch (setValue) {
    | Some(setValue) => (value, setValue)
    | None => innerValue
    };

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
