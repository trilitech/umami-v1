let formatOnBlur = (token, handleChange, value) =>
  if (token->Option.isSome) {
    value
    ->Token.Unit.forceFromString
    ->Option.mapWithDefault("", Token.Unit.toNatString)
    ->handleChange;
  } else {
    value->ProtocolXTZ.formatString->Option.getWithDefault("")->handleChange;
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
      ~token: option(Token.t)=?,
    ) => {
  // reformat value if token change
  React.useEffect1(
    () => {
      if (value != "") {
        formatOnBlur(token, handleChange, value);
      };
      None;
    },
    [|token|],
  );

  let placeholder =
    token == None
      ? I18n.input_placeholder#tez_amount : I18n.input_placeholder#token_amount;

  <FormGroupTextInput
    label
    ?style
    placeholder
    value
    error
    onBlur={_ => formatOnBlur(token, handleChange, value)}
    ?decoration
    handleChange
    keyboardType=`numeric
  />;
};
