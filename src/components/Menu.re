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
let make =
    (
      ~keyMenu="menu",
      ~icon: Icons.builder,
      ~children,
      ~size=34.,
      ~style as styleArg=?,
    ) => {
  let pressableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);
  let (config, setConfig) = React.useState(_ => None);

  DocumentContext.useClickOutside(
    pressableRef,
    isOpen,
    React.useCallback1(_pressEvent => setIsOpen(_ => false), [|setIsOpen|]),
  );

  let onPress = _ => {
    pressableRef.current
    ->Js.Nullable.toOption
    ->Option.map(pressableElement => {
        pressableElement->PressableCustom.measureInWindow(
          (x, y, width, height) => {
          setConfig(_ => Some(PressableCustom.{x, y, width, height}))
        })
      })
    ->ignore;
    setIsOpen(isOpen => !isOpen);
  };

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
      onPress
    />
    <Portal>
      <DropdownMenu
        key=keyMenu
        openingStyle=DropdownMenu.TopRight
        style=styles##dropdownmenu
        isOpen
        ?config>
        children
      </DropdownMenu>
    </Portal>
  </View>;
};
