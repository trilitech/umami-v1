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
      <Typography.ButtonSecondary colorStyle style=styles##text>
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
          ~position=`absolute,
          ~top=2.->dp,
          ~right=2.->dp,
          ~minWidth=170.->dp,
          ~maxHeight=224.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~icon: Icons.builder, ~children, ~size=34., ~style as styleArg=?) => {
  let pressableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);

  DocumentContext.useClickOutside(
    pressableRef,
    isOpen,
    React.useCallback1(_pressEvent => setIsOpen(_ => false), [|setIsOpen|]),
  );

  <View
    style=Style.(
      arrayOption([|
        Some(style(~width=size->dp, ~height=size->dp, ())),
        styleArg,
      |])
    )>
    <IconButton
      pressableRef={pressableRef->Ref.value}
      isActive=isOpen
      icon
      size
      onPress={_ => setIsOpen(isOpen => !isOpen)}
    />
    <DropdownMenu
      openingStyle=DropdownMenu.TopRight style=styles##dropdownmenu isOpen>
      children
    </DropdownMenu>
  </View>;
};
