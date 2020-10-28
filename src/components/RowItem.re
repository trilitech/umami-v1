open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~paddingVertical=6.->dp, ~flexDirection=`row, ()),
      "containerHovered": style(~backgroundColor=Theme.colorDarkSelected, ()),
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
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

[@react.component]
let make = (~height, ~children) => {
  <Pressable>
    {interactionState =>
       <View
         style=Style.(
           arrayOption([|
             Some(styles##container),
             Some(style(~height=height->dp, ())),
             interactionState.hovered ? Some(styles##containerHovered) : None,
           |])
         )>
         <View style=styles##border />
         <View style=styles##inner> {children(interactionState)} </View>
       </View>}
  </Pressable>;
};
