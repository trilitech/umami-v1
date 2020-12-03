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
      "buttonHovered": style(~backgroundColor="rgba(255,255,255,0.04)", ()),
    })
  );

[@react.component]
let make = (~icon: Icons.builder, ~hoveredStyle=?, ~onPress=?) => {
  <PressableCustom ?onPress>
    {({hovered}) =>
       <View
         style=Style.(
           arrayOption([|
             Some(styles##button),
             hovered ? Some(styles##buttonHovered) : None,
             hovered ? hoveredStyle : None,
           |])
         )>
         {icon(~style=?None, ~size=16., ~color=Theme.colorDarkMediumEmphasis)}
       </View>}
  </PressableCustom>;
};
