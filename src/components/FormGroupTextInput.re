open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "label": style(~marginBottom=6.->dp, ()),
      "decoration":
        style(
          ~display=`flex,
          ~alignItems=`center,
          ~position=`absolute,
          ~marginTop=auto,
          ~marginBottom=auto,
          ~top=0.->dp,
          ~bottom=0.->dp,
          ~right=10.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~handleChange,
      ~error,
      ~keyboardType=?,
      ~onBlur=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~placeholder=?,
      ~decoration: option((~style: Style.t) => React.element)=?,
      ~style: option(ReactNative.Style.t)=?,
    ) => {
  let hasError = error->Belt.Option.isSome;
  <FormGroup ?style>
    <FormLabel label hasError style=styles##label />
    <View>
      <ThemedTextInput
        value
        onValueChange=handleChange
        hasError
        ?onBlur
        ?textContentType
        ?secureTextEntry
        ?keyboardType
        ?placeholder
      />
      {decoration->ReactUtils.mapOpt(deco => deco(~style=styles##decoration))}
    </View>
  </FormGroup>;
};
