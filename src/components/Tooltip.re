open ReactNative;
let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~padding=5.->dp,
          ~width=120.->dp,
          ~borderRadius=4.,
          ~display=`flex,
          ~justifyContent=`center,
          ~alignItems=`center,
          ~borderWidth=1.,
          (),
        ),
      "position":
        style(~top=15.->dp, ~right=50.->pct, ~position=`absolute, ()),
    })
  );

[@react.component]
let make = (~keyPopover, ~config, ~text, ~isOpen) => {
  let theme = ThemeContext.useTheme();
  <Popover
    openingStyle=Popover.Top
    style=styles##position
    pointerEvents=`none
    keyPopover
    isOpen
    config>
    <View
      style=Style.(
        array([|
          styles##container,
          style(
            ~backgroundColor=theme.colors.barBackground,
            ~borderColor=theme.colors.borderMediumEmphasis,
            (),
          ),
        |])
      )>
      <Typography.Overline2> text->React.string </Typography.Overline2>
    </View>
  </Popover>;
};
