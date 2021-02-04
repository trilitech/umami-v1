open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~alignSelf=`flexStart,
          ~height=37.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          (),
        ),
      "radio": style(~marginRight=16.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value,
      ~setValue: (ThemeContext.themeMain => ThemeContext.themeMain) => unit,
      ~currentValue,
    ) => {
  <TouchableOpacity
    style=styles##container onPress={_ => setValue(_ => value)}>
    <Radio value={currentValue == value} style=styles##radio />
    <Typography.Body1> label->React.string </Typography.Body1>
  </TouchableOpacity>;
};
