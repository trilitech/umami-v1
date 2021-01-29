open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button": style(~alignItems=`center, ~justifyContent=`center, ()),
    })
  );

let iconSizeRatio = 4. /. 7.;

[@react.component]
let make =
    (
      ~icon: Icons.builder,
      ~size=28.,
      ~isPrimary=?,
      ~onPress=?,
      ~isActive=?,
      ~pressableRef=?,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let theme = ThemeContext.useTheme();
  <ThemedPressable
    ?pressableRef
    ?onPress
    ?isPrimary
    ?isActive
    style=Style.(
      arrayOption([|
        Some(styles##button),
        Some(
          style(
            ~width=size->dp,
            ~height=size->dp,
            ~borderRadius=size /. 2.,
            (),
          ),
        ),
        styleFromProp,
      |])
    )
    accessibilityRole=`button>
    {icon(
       ~style=?None,
       ~size=Js.Math.ceil_float(iconSizeRatio *. size),
       ~color=
         isPrimary->Option.getWithDefault(false)
           ? theme.colors.primaryIconMediumEmphasis
           : theme.colors.iconMediumEmphasis,
     )}
  </ThemedPressable>;
};
