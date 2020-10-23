open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "label":
        style(
          ~marginBottom=6.->dp,
          ~color=Colors.stdText,
          ~fontSize=18.,
          ~fontWeight=`_400,
          (),
        ),
      "labelError": style(~color=Colors.error, ()),
      "labelSmall": style(~marginBottom=4.->dp, ~fontSize=16., ()),
    })
  );

[@react.component]
let make = (~label, ~hasError=false, ~small=false) => {
  <Text
    style=Style.(
      arrayOption([|
        Some(styles##label),
        hasError ? Some(styles##labelError) : None,
        small ? Some(styles##labelSmall) : None,
      |])
    )>
    label->React.string
  </Text>;
};
