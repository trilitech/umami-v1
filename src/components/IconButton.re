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
  <PressableCustom ?onPress>
    {({hovered, pressed}) => {
       <View
         style=Style.(
           arrayOption([|
             Some(styles##button),
             hovered
               ? Some(
                   Style.style(
                     ~backgroundColor=theme.colors.stateHovered,
                     (),
                   ),
                 )
               : None,
             pressed
               ? Some(
                   Style.style(
                     ~backgroundColor=theme.colors.statePressed,
                     (),
                   ),
                 )
               : None,
             hovered ? hoveredStyle : None,
           |])
         )>
         {icon(
            ~style=?None,
            ~size=16.,
            ~color=theme.colors.iconMediumEmphasis,
          )}
       </View>;
     }}
  </PressableCustom>;
};
