open ReactNative;

let listVerticalPadding = 8.;

let styles =
  Style.(
    StyleSheet.create({
      "listContainer":
        style(~borderRadius=3., ())
        ->unsafeAddStyle({
            "boxShadow": "0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 3px 14px 2px rgba(0, 0, 0, 0.12), 0 8px 10px 1px rgba(0, 0, 0, 0.14)",
          }),

      "positionBottomTop":
        style(~bottom=35.->dp, ~top="unset", ~position=`absolute, ()),

      "listContentContainer":
        style(~paddingVertical=listVerticalPadding->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~scrollRef=?,
      ~isOpen,
      ~popoverConfig: option(Popover.targetLayout),
      ~openingStyle=Popover.Top,
      ~style as styleFromProp=?,
      ~onScroll=?,
      ~scrollEventThrottle=?,
      ~onRequestClose: unit => unit,
      ~keyPopover,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  let dimensions = Dimensions.useWindowDimensions();

  let position =
    popoverConfig->Option.flatMap(config =>
      config.y /. dimensions.height > 0.80
        ? Some(styles##positionBottomTop) : None
    );

  <Popover
    isOpen
    openingStyle
    config=popoverConfig
    style={Style.arrayOption([|styleFromProp, position|])}
    keyPopover>
    <View
      onStartShouldSetResponderCapture={_ => true}
      onResponderRelease={_ => onRequestClose()}>
      <ScrollView
        ref=?scrollRef
        style=Style.(
          array([|
            styles##listContainer,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )
        contentContainerStyle=Style.(
          arrayOption([|
            Some(styles##listContentContainer),
            theme.dark
              ? Some(style(~backgroundColor=theme.colors.stateActive, ()))
              : None,
          |])
        )
        ?onScroll
        ?scrollEventThrottle>
        children
      </ScrollView>
    </View>
  </Popover>;
};
