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
          ~textAlign=`center,
          ~borderWidth=1.,
          (),
        ),
      "positionLeft":
        style(
          ~top=15.->dp,
          ~right=0.->dp,
          ~left="unset",
          ~position=`absolute,
          (),
        ),
      "positionRight":
        style(
          ~top=15.->dp,
          ~right="unset",
          ~left=0.->dp,
          ~position=`absolute,
          (),
        ),
    })
  );

[@react.component]
let make =
    (~keyPopover, ~config: option(Popover.targetLayout), ~text, ~isOpen) => {
  let theme = ThemeContext.useTheme();

  let dimensions = Dimensions.useWindowDimensions();

  let position =
    config->Option.mapWithDefault(styles##positionRight, config =>
      config.x /. dimensions.width > 0.75
        ? styles##positionLeft : styles##positionRight
    );

  <Popover
    openingStyle=Popover.Top
    style=position
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
