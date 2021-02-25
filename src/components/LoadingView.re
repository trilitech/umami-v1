open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flex=1., ~alignItems=`center, ~justifyContent=`center, ()),
    })
  );

[@react.component]
let make = (~color=?, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  <View style=Style.(arrayOption([|Some(styles##container), styleArg|]))>
    <ActivityIndicator
      animating=true
      size=ActivityIndicator_Size.small
      color={color->Option.getWithDefault(theme.colors.iconHighEmphasis)}
    />
  </View>;
};
