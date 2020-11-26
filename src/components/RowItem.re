open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flexDirection=`row, ()),
      "containerHovered": style(~backgroundColor=Theme.colorDarkSelected, ()),
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "innerContainer":
        style(~paddingVertical=6.->dp, ~flexDirection=`row, ~flex=1., ()),
      "border":
        style(
          ~backgroundColor="#8D9093",
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
               hovered ? Some(styles##containerHovered) : None,
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
    <Base height ?style>
      {_ =>
         <>
           <View style=styles##border />
           <View style=styles##inner> children </View>
         </>}
    </Base>;
  };
};
