open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button": style(~alignItems=`center, ~justifyContent=`center, ()),
      "popover":
        style(~top=(-100.)->dp, ~right=2.->dp, ~position=`absolute, ()),
      "hoverable": style(~position=`relative, ()),
    })
  );

[@react.component]
let make =
    (
      ~icon: Icons.builder,
      ~size=28.,
      ~iconSizeRatio=4. /. 7.,
      ~isPrimary=?,
      ~onPress=?,
      ~tooltip=?,
      ~isActive=?,
      ~pressableRef=?,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  let theme = ThemeContext.useTheme();
  let (pressableRef, isOpen, popoverConfig, togglePopover, setClosed) =
    Popover.usePopoverState(~elementRef=?pressableRef, ());

  <View onMouseEnter={_ => togglePopover()} onMouseLeave={_ => setClosed()}>
    {ReactUtils.mapOpt(tooltip, ((keyPopover, text)) => {
       <Tooltip keyPopover text isOpen config=popoverConfig />
     })}
    <ThemedPressable
      pressableRef={pressableRef->Ref.value}
      ?onPress
      ?isPrimary
      ?isActive
      style=Style.(
        arrayOption([|
          Some(styles##button),
          Some(
            style(
              ~width=size->dp,
              ~height=size->dp,
              ~borderRadius=size /. 2.,
              (),
            ),
          ),
          styleFromProp,
        |])
      )
      accessibilityRole=`button>
      {icon(
         ~style=?None,
         ~size=Js.Math.ceil_float(iconSizeRatio *. size),
         ~color=
           isPrimary->Option.getWithDefault(false)
             ? theme.colors.primaryIconMediumEmphasis
             : theme.colors.iconMediumEmphasis,
       )}
    </ThemedPressable>
  </View>;
};
