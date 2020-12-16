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
let make = (~icon: Icons.builder, ~hoveredStyle=?, ~onPress=?) => {
  let theme = ThemeContext.useTheme();
  <ThemedPressable
    ?onPress
    style=styles##button
    interactionStyle={({hovered}) => hovered ? hoveredStyle : None}>
    {icon(~style=?None, ~size=16., ~color=theme.colors.iconMediumEmphasis)}
  </ThemedPressable>;
};
