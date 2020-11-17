open ReactNative;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~height=30.->dp,
            ~paddingHorizontal=14.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
        "buttonHovered": style(~backgroundColor="rgba(255,255,255,0.04)", ()),
        "text": style(~marginLeft=10.->dp, ()),
      })
    );

  [@react.component]
  let make = (~text, ~icon: Icons.builder, ~onPress=?) => {
    <Pressable ?onPress>
      {({hovered}) =>
         <View
           style=Style.(
             arrayOption([|
               Some(styles##button),
               hovered ? Some(styles##buttonHovered) : None,
             |])
           )>
           {icon(
              ~style=?None,
              ~size=20.,
              ~color=Theme.colorDarkMediumEmphasis,
            )}
           <Typography.ButtonSecondary
             colorStyle=`highEmphasis style=styles##text>
             text->React.string
           </Typography.ButtonSecondary>
         </View>}
    </Pressable>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~width=42.->dp,
          ~height=42.->dp,
          ~marginRight=4.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderRadius=21.,
          (),
        ),
      "buttonHovered": style(~backgroundColor="rgba(255,255,255,0.04)", ()),
      "buttonPressed": style(~backgroundColor="rgba(255,255,255,0.1)", ()),
      "listContainer":
        style(
          ~position=`absolute,
          ~top=2.->dp,
          ~right=2.->dp,
          ~minWidth=170.->dp,
          ~maxHeight=224.->dp,
          ~paddingVertical=8.->dp,
          ~backgroundColor="#2e2e2e",
          ~borderRadius=3.,
          (),
        ),
    })
  );

[@react.component]
let make = (~icon: Icons.builder, ~children) => {
  let pressableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);

  DocumentContext.useDocumentPress(
    React.useCallback1(
      pressEvent =>
        if (pressableRef.current !==
            pressEvent->Event.PressEvent.nativeEvent##target) {
          setIsOpen(_ => false);
        },
      [|setIsOpen|],
    ),
  );

  <View>
    <Pressable
      ref={pressableRef->Ref.value}
      onPress={_ => setIsOpen(isOpen => !isOpen)}>
      {({hovered}) =>
         <View
           style=Style.(
             arrayOption([|
               Some(styles##button),
               hovered ? Some(styles##buttonHovered) : None,
               isOpen ? Some(styles##buttonPressed) : None,
             |])
           )
           pointerEvents=`none>
           {icon(
              ~style=?None,
              ~size=24.,
              ~color=Theme.colorDarkMediumEmphasis,
            )}
         </View>}
    </Pressable>
    <View style={ReactUtils.displayOn(isOpen)}>
      <ScrollView style=styles##listContainer> children </ScrollView>
    </View>
  </View>;
};
