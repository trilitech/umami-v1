open ReactNative;
include NativeElement;

let styles =
  Style.(StyleSheet.create({"container": style(~overflow=`hidden, ())}));

[@react.component]
let make =
    (
      ~pressableRef: option(ref)=?,
      ~onPress=?,
      ~href=?,
      ~style as styleFromProp=?,
      ~outerStyle=?,
      ~interactionStyle:
         option(Pressable_.interactionState => option(ReactNative.Style.t))=?,
      ~isActive=false,
      ~disabled: option(bool)=?,
      ~isPrimary=false,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();
  <Pressable_
    style=?outerStyle
    ref=?pressableRef
    ?onPress
    ?disabled
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
             hovered || focused
               ? Some(
                   Style.style(
                     ~backgroundColor=
                       isPrimary
                         ? theme.colors.primaryStateHovered
                         : theme.colors.stateHovered,
                     (),
                   ),
                 )
               : None,
             pressed
               ? Some(
                   Style.style(
                     ~backgroundColor=
                       isPrimary
                         ? theme.colors.primaryStatePressed
                         : theme.colors.statePressed,
                     (),
                   ),
                 )
               : None,
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
