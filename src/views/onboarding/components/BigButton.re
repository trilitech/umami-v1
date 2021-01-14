open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=203.->dp,
          ~width=424.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderRadius=4.,
          (),
        )
        ->unsafeAddStyle({
            "boxShadow": "0 1px 3px 0 rgba(0, 0, 0, 0.2), 0 2px 1px -1px rgba(0, 0, 0, 0.12), 0 1px 1px 0 rgba(0, 0, 0, 0.14)",
          }),
      "iconContainer":
        style(
          ~width=60.->dp,
          ~height=60.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~marginBottom=28.->dp,
          ~borderRadius=30.,
          (),
        ),
    })
  );

[@react.component]
let make = (~title, ~icon: Icons.builder, ~onPress) => {
  let theme = ThemeContext.useTheme();

  <TouchableOpacity
    style=Style.(
      array([|
        styles##button,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )
    onPress>
    <View
      style=Style.(
        array([|
          styles##iconContainer,
          style(~backgroundColor=theme.colors.primaryButtonBackground, ()),
        |])
      )>
      {icon(
         ~color=theme.colors.primaryIconHighEmphasis,
         ~size=36.,
         ~style=?None,
       )}
    </View>
    <Typography.Subtitle2> title->React.string </Typography.Subtitle2>
  </TouchableOpacity>;
};
