open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        StyleSheet.flatten([|
          FormStyles.square(40.),
          style(
            ~alignItems=`center,
            ~justifyContent=`center,
            ~borderRadius=20.,
            (),
          ),
          ShadowStyles.button,
        |]),
    })
  );

[@react.component]
let make = (~onRefresh, ~loading, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  <View
    style=Style.(
      arrayOption([|
        Some(styles##container),
        Some(style(~backgroundColor=theme.colors.elevatedBackground, ())),
        styleArg,
      |])
    )>
    {loading
       ? <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.small
           color={theme.colors.iconHighEmphasis}
         />
       : <IconButton
           size=40.
           icon=Icons.Refresh.build
           onPress={_ => onRefresh()}
         />}
  </View>;
};
