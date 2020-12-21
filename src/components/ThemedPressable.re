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
      ~interactionStyle:
         option(
           PressableCustom.interactionState => option(ReactNative.Style.t),
         )=?,
      ~isActive=false,
      ~disabled: option(bool)=?,
      ~isPrimary=false,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();
  <PressableCustom
    ref=?pressableRef ?onPress ?disabled ?href ?accessibilityRole>
    {({hovered, pressed, focused} as interactionState) => {
       <View
         pointerEvents=`none
         style=Style.(
           arrayOption([|
             Some(styles##container),
             styleFromProp,
             hovered
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
             pressed || focused
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
             interactionStyle->Belt.Option.flatMap(interactionStyle =>
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
  </PressableCustom>;
};
