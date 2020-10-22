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
      "text":
        style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`_600, ()),
      "textDisabled":
        style(
          ~color=Colors.disabledText,
          ~fontSize=14.,
          ~fontWeight=`_600,
          (),
        ),
    })
  );

[@react.component]
let make = (~text, ~onPress, ~disabled=?) => {
  <TouchableOpacity style=styles##button onPress ?disabled>
    <Text
      style=Style.(
        arrayOption([|
          Some(styles##text),
          disabled->Belt.Option.flatMap(disabled =>
            disabled ? Some(styles##textDisabled) : None
          ),
        |])
      )>
      text->React.string
    </Text>
  </TouchableOpacity>;
};
