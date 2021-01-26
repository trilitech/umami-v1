open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "pressable":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingVertical=6.->dp,
          ~paddingLeft=6.->dp,
          ~paddingRight=9.->dp,
          ~borderRadius=5.,
          (),
        ),
      "icon": style(~marginRight=4.->dp, ()),
    })
  );

[@react.component]
let make = (~text, ~onPress, ~icon: Icons.builder) => {
  let theme = ThemeContext.useTheme();

  <ThemedPressable style=styles##pressable onPress accessibilityRole=`button>
    {icon(
       ~style=styles##icon,
       ~size=15.5,
       ~color=theme.colors.iconMediumEmphasis,
     )}
    <Typography.ButtonSecondary>
      text->React.string
    </Typography.ButtonSecondary>
  </ThemedPressable>;
};
