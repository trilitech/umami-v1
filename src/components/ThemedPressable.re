open ReactNative;
include NativeElement;

[@react.component]
let make =
    (
      ~pressableRef: option(ref)=?,
      ~onPress=?,
      ~style as styleFromProp=?,
      ~interactionStyle:
         option(
           PressableCustom.interactionState => option(ReactNative.Style.t),
         )=?,
      ~disabled: option(bool)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();
  <PressableCustom ref=?pressableRef ?onPress ?disabled>
    {({hovered, pressed} as interactionState) => {
       <View
         pointerEvents=`none
         style=Style.(
           arrayOption([|
             styleFromProp,
             hovered
               ? Some(
                   Style.style(
                     ~backgroundColor=theme.colors.stateHovered,
                     (),
                   ),
                 )
               : None,
             pressed
               ? Some(
                   Style.style(
                     ~backgroundColor=theme.colors.statePressed,
                     (),
                   ),
                 )
               : None,
             interactionStyle->Belt.Option.flatMap(interactionStyle =>
               interactionStyle(interactionState)
             ),
           |])
         )>
         children
       </View>;
     }}
  </PressableCustom>;
};
