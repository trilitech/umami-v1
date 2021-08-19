open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "tag":
        style(
          ~width=40.->dp,
          ~height=18.->dp,
          ~borderRadius=9.,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderWidth=1.,
          (),
        ),
    })
  );

[@react.component]
let make = (~style as styleProp=?, ~content: string) => {
  let theme = ThemeContext.useTheme();

  <View
    style=Style.(
      arrayOption([|
        styles##tag->Some,
        style(~backgroundColor=theme.colors.statePressed, ())->Some,
        styleProp,
      |])
    )>
    <Typography.Body2 fontSize=9.7 colorStyle=`mediumEmphasis>
      content->React.string
    </Typography.Body2>
  </View>;
};
