open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~flexDirection=`row, ~alignItems=`center, ()),
      "touchable":
        style(
          ~alignSelf=`flexStart,
          ~flexDirection=`row,
          ~alignItems=`center,
          (),
        ),
      "checkbox": style(~marginRight=14.->dp, ()),
    })
  );

[@react.component]
let make = (~label, ~value, ~handleChange, ~error) => {
  let hasError = error->Option.isSome;
  <FormGroup>
    <TouchableOpacity
      onPress={_ => handleChange(!value)}
      style=styles##touchable
      activeOpacity=1.>
      <Checkbox value style=styles##checkbox />
      <FormLabel label hasError />
    </TouchableOpacity>
  </FormGroup>;
};
