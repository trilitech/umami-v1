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
        "text": style(~marginLeft=10.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (~text, ~icon: Icons.builder, ~colorStyle=`highEmphasis, ~onPress=?) => {
    let theme = ThemeContext.useTheme();
    <ThemedPressable ?onPress style=styles##button accessibilityRole=`button>
      {icon(
         ~style=?None,
         ~size=20.,
         ~color=
           (colorStyle === `highEmphasis ? `mediumEmphasis : colorStyle)
           ->Typography.getColor(theme),
       )}
      <Typography.ButtonSecondary colorStyle fontSize=14. style=styles##text>
        text->React.string
      </Typography.ButtonSecondary>
    </ThemedPressable>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "dropdownmenu":
        style(
          ~top=2.->dp,
          ~right=2.->dp,
          ~minWidth=170.->dp,
          ~maxHeight=224.->dp,
          (),
        ),
    })
  );

[@react.component]
let make =
    (
      ~keyPopover,
      ~icon: Icons.builder,
      ~children,
      ~size=34.,
      ~style as styleArg=?,
    ) => {
  let (pressableRef, isOpen, popoverConfig, togglePopover, _) =
    Popover.usePopoverState();

  <View
    style=Style.(
      arrayOption([|
        Some(style(~width=size->dp, ~height=size->dp, ())),
        styleArg,
      |])
    )>
    <IconButton
      pressableRef
      isActive=isOpen
      icon
      size
      onPress={_ => togglePopover()}
    />
    <DropdownMenu
      keyPopover
      openingStyle=Popover.TopRight
      style=styles##dropdownmenu
      isOpen
      popoverConfig
      onRequestClose=togglePopover>
      {_ => children}
    </DropdownMenu>
  </View>;
};
