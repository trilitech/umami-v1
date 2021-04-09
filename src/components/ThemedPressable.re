open ReactNative;

let styles =
  Style.(StyleSheet.create({"container": style(~overflow=`hidden, ())}));

[@react.component]
let make =
    (
      ~pressableRef: option(NativeElement.ref)=?,
      ~onPress=?,
      ~href=?,
      ~style as styleFromProp=?,
      ~outerStyle=?,
      ~interactionStyle:
         option(Pressable_.interactionState => option(ReactNative.Style.t))=?,
      ~isActive=false,
      ~disabled=false,
      ~isPrimary=false,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  let backgroundColor = (~pressed, ~hovered, ~focused) =>
    if (disabled) {
      isPrimary
        ? Some(theme.colors.primaryStateDisabled)
        : Some(theme.colors.stateDisabled);
    } else if (hovered || focused) {
      isPrimary
        ? Some(theme.colors.primaryStateHovered)
        : Some(theme.colors.stateHovered);
    } else if (pressed) {
      isPrimary
        ? Some(theme.colors.primaryStatePressed)
        : Some(theme.colors.statePressed);
    } else {
      None;
    };

  <Pressable_
    style=?outerStyle
    ref=?pressableRef
    ?onPress
    disabled
    ?href
    ?accessibilityRole>
    {({hovered, pressed, focused} as interactionState) => {
       let hovered = hovered->Option.getWithDefault(false);
       let focused = focused->Option.getWithDefault(false);
       <View
         pointerEvents=`none
         style=Style.(
           arrayOption([|
             Some(styles##container),
             styleFromProp,
             backgroundColor(~pressed, ~hovered, ~focused)
             ->Option.map(b => Style.(style(~backgroundColor=b, ()))),
             interactionStyle->Option.flatMap(interactionStyle =>
               interactionStyle(interactionState)
             ),
           |])
         )>
         <View
           style=Style.(
             arrayOption([|
               Some(StyleSheet.absoluteFillObject),
               isActive
                 ? Some(
                     Style.style(
                       ~backgroundColor=
                         isPrimary
                           ? theme.colors.primaryStateActive
                           : theme.colors.stateActive,
                       (),
                     ),
                   )
                 : None,
             |])
           )
         />
         children
       </View>;
     }}
  </Pressable_>;
};
