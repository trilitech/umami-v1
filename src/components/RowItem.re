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
    <View
      style=Style.(
        arrayOption([|
          stylearg,
          Some(styles##container),
          Some(style(~height=height->dp, ())),
        |])
      )>
      <View style=styles##innerContainer> children </View>
    </View>;
  };
};

module Bordered = {
  [@react.component]
  let make = (~height, ~style=?, ~children) => {
    <Base height ?style>
      <View style=styles##border />
      <View style=styles##inner> children </View>
    </Base>;
  };
};

module Pressable = {
  [@react.component]
  let make = (~height, ~style=?, ~children) => {
    <Pressable>
      {interactionState =>
         <Base ?style height>
           <View style=styles##inner> {children(interactionState)} </View>
         </Base>}
    </Pressable>;
  };
};
