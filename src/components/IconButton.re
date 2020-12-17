open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~width=28.->dp,
          ~height=28.->dp,
          ~marginRight=4.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderRadius=14.,
          (),
        ),
    })
  );

[@react.component]
let make = (~icon: Icons.builder, ~isPrimary=?, ~onPress=?) => {
  let theme = ThemeContext.useTheme();
  <ThemedPressable ?onPress style=styles##button ?isPrimary>
    {icon(
       ~style=?None,
       ~size=16.,
       ~color=
         isPrimary->Belt.Option.getWithDefault(false)
           ? theme.colors.primaryIconMediumEmphasis
           : theme.colors.iconMediumEmphasis,
     )}
  </ThemedPressable>;
};
