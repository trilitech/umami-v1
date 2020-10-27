open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~width=160.->dp,
          ~height=46.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~text, ~onPress, ~disabled=?) => {
  <TouchableOpacity style=styles##button onPress ?disabled>
    <Typography.ButtonPrimary
      colorStyle=?{
        disabled->Belt.Option.flatMap(disabled =>
          disabled ? Some(`disabled) : None
        )
      }>
      text->React.string
    </Typography.ButtonPrimary>
  </TouchableOpacity>;
};
