open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flexDirection=`row, ()),
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "innerContainer":
        style(~paddingVertical=6.->dp, ~flexDirection=`row, ~flex=1., ()),
      "border":
        style(
          ~width=2.->dp,
          ~borderTopRightRadius=2.,
          ~borderBottomRightRadius=2.,
          (),
        ),
    })
  );

module Base = {
  [@react.component]
  let make = (~height, ~style as stylearg=?, ~children) => {
    let theme = ThemeContext.useTheme();
    <Hoverable
      style=Style.(
        arrayOption([|
          stylearg,
          Some(styles##container),
          Some(style(~height=height->dp, ())),
        |])
      )>
      {hovered => {
         <View
           style=Style.(
             arrayOption([|
               Some(styles##innerContainer),
               hovered
                 ? Some(
                     style(~backgroundColor=theme.colors.stateHovered, ()),
                   )
                 : None,
             |])
           )>
           {children(hovered)}
         </View>;
       }}
    </Hoverable>;
  };
};

module Bordered = {
  [@react.component]
  let make = (~height, ~style=?, ~children) => {
    let theme = ThemeContext.useTheme();
    <Base height ?style>
      {_ =>
         <>
           <View
             style=Style.(
               array([|
                 styles##border,
                 style(~backgroundColor=theme.colors.borderHighEmphasis, ()),
               |])
             )
           />
           <View style=styles##inner> children </View>
         </>}
    </Base>;
  };
};
