open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~flexDirection=`row, ()),
      "containerHovered": style(~backgroundColor=Theme.colorDarkSelected, ()),
      "inner": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "innerContainer":
        style(
          ~paddingVertical=6.->dp,
          ~flexDirection=`row,
          ~width=100.->pct,
          (),
        ),
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
  let make = (~height, ~style=?, ~children) => {
    let stylearg = style;
    <Pressable>
      {interactionState =>
         <View
           style=Style.(
             arrayOption([|
               stylearg,
               Some(styles##container),
               Some(style(~height=height->dp, ())),
             |])
           )>
           <View
             style=Style.(
               arrayOption([|
                 Some(styles##innerContainer),
                 interactionState.hovered
                   ? Some(styles##containerHovered) : None,
               |])
             )>
             {children(interactionState)}
           </View>
         </View>}
    </Pressable>;
  };
};

module Bordered = {
  [@react.component]
  let make = (~height, ~style=?, ~children) => {
    <Base height ?style>
      {interactionState =>
         <>
           {<View style=styles##border />}
           <View style=styles##inner> {children(interactionState)} </View>
         </>}
    </Base>;
  };
};

[@react.component]
let make = (~height, ~style=?, ~children) => {
  <Base ?style height>
    {interactionState =>
       <> <View style=styles##inner> {children(interactionState)} </View> </>}
  </Base>;
};
