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
let make =
    (
      ~text,
      ~onPress,
      ~tooltip=?,
      ~disabled=?,
      ~icon: Icons.builder,
      ~primary=false,
    ) => {
  let theme = ThemeContext.useTheme();

  <ThemedPressable
    style=styles##pressable
    ?tooltip
    ?disabled
    onPress
    accessibilityRole=`button>
    {icon(
       ~style=styles##icon,
       ~size=15.5,
       ~color=
         primary ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis,
     )}
    <Typography.ButtonSecondary
      style=Style.(
        style(
          ~color=?{
            primary ? Some(theme.colors.iconPrimary) : None;
          },
          (),
        )
      )>
      text->React.string
    </Typography.ButtonSecondary>
  </ThemedPressable>;
};
