open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~marginBottom=0.->dp, ()),
      "label": style(~marginVertical=4.->dp, ()),
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
      ~onFocus=?,
      ~textContentType=?,
      ~secureTextEntry=?,
      ~placeholder=?,
      ~disabled=?,
      ~clearButton=false,
      ~onSubmitEditing=?,
      ~decoration: option((~style: Style.t) => React.element)=?,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let hasError = error->Option.isSome;
  <FormGroup
    style=Style.(arrayOption([|Some(styles##formGroup), styleFromProp|]))>
    <FormLabel label hasError style=styles##label />
    <View>
      <ThemedTextInput
        value
        onValueChange=handleChange
        hasError
        ?onBlur
        ?onFocus
        ?textContentType
        ?secureTextEntry
        ?keyboardType
        ?placeholder
        ?disabled
        ?onSubmitEditing
        onClear=?{clearButton ? Some(() => handleChange("")) : None}
      />
      {decoration->ReactUtils.mapOpt(deco => deco(~style=styles##decoration))}
    </View>
    <FormError ?error />
  </FormGroup>;
};
