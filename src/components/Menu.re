open ReactNative;
open Common;

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
    <ThemedPressable ?onPress style=styles##button>
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
      "button": style(~alignItems=`center, ~justifyContent=`center, ()),
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

let iconSizeRatio = 4. /. 7.;

[@react.component]
let make = (~icon: Icons.builder, ~children, ~size=42., ~style as styleArg=?) => {
  let pressableRef = React.useRef(Js.Nullable.null);

  let (isOpen, setIsOpen) = React.useState(_ => false);

  DocumentContext.useClickOutside(
    pressableRef,
    isOpen,
    React.useCallback1(_pressEvent => setIsOpen(_ => false), [|setIsOpen|]),
  );

  let theme = ThemeContext.useTheme();

  <View
    style={
      (Style.(style(~width=size->dp, ~height=size->dp, ())) @$? styleArg)
      ->ReactUtils.styles
    }>
    <ThemedPressable
      pressableRef={pressableRef->Ref.value}
      onPress={_ => setIsOpen(isOpen => !isOpen)}
      style=Style.(
        array([|
          styles##button,
          style(
            ~width=size->dp,
            ~height=size->dp,
            ~borderRadius=size /. 2.,
            (),
          ),
        |])
      )
      isActive=isOpen>
      {icon(
         ~style=?None,
         ~size=Js.Math.ceil_float(iconSizeRatio *. size),
         ~color=theme.colors.iconMediumEmphasis,
       )}
    </ThemedPressable>
    <DropdownMenu style=styles##dropdownmenu isOpen> children </DropdownMenu>
  </View>;
};
