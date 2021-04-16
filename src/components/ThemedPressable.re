open ReactNative;

let styles =
  Style.(StyleSheet.create({"container": style(~overflow=`hidden, ())}));

[@react.component]
let make =
    (
      ~pressableRef=?,
      ~onPress=?,
      ~href=?,
      ~style as styleFromProp=?,
      ~outerStyle=?,
      ~tooltip=?,
      ~interactionStyle:
         option(Pressable_.interactionState => option(ReactNative.Style.t))=?,
      ~isActive=false,
      ~disabled=false,
      ~isPrimary=false,
      ~accessibilityRole: option(Accessibility.role)=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  let (pressableRef, isOpen, popoverConfig, togglePopover, setClosed) =
    Popover.usePopoverState(~elementRef=?pressableRef, ());

  let backgroundColor = (~pressed, ~hovered, ~focused) =>
    if (disabled) {
      isPrimary ? Some(theme.colors.primaryStateDisabled) : None;
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

  <View
    style=?outerStyle
    onMouseEnter={_ => togglePopover()}
    onMouseLeave={_ => setClosed()}>
    <Pressable_
      ref={pressableRef->Ref.value} ?onPress disabled ?href ?accessibilityRole>
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
           {ReactUtils.mapOpt(tooltip, ((keyPopover, text)) => {
              <Tooltip keyPopover text isOpen config=popoverConfig />
            })}
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
    </Pressable_>
  </View>;
};
