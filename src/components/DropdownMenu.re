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
      "listContentContainer":
        style(~paddingVertical=listVerticalPadding->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~scrollRef=?,
      ~isOpen=false,
      ~style as styleFromProp=?,
      ~onScroll=?,
      ~scrollEventThrottle=?,
      ~children,
    ) => {
  let theme = ThemeContext.useTheme();

  <View style={ReactUtils.displayOn(isOpen)}>
    <ScrollView
      ref=?scrollRef
      style=Style.(
        arrayOption([|
          Some(styles##listContainer),
          Some(style(~backgroundColor=theme.colors.background, ())),
          styleFromProp,
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
  </View>;
};
