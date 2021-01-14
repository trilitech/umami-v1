open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          //~marginRight=4.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
    })
  );

let iconSizeRatio = 5. /. 7.;

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
    )>
    {icon(
       ~style=?None,
       ~size=Js.Math.ceil_float(iconSizeRatio *. size),
       ~color=
         isPrimary->Belt.Option.getWithDefault(false)
           ? theme.colors.primaryIconMediumEmphasis
           : theme.colors.iconMediumEmphasis,
     )}
  </ThemedPressable>;
};
