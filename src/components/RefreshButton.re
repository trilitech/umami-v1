open ReactNative;

[@react.component]
let make = (~onRefresh, ~loading, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  let style =
    Style.(
      arrayOption([|
        Some(SendButton.styles##button),
        styleArg,
        Some(style(~backgroundColor=theme.colors.elevatedBackground, ())),
      |])
    );

  loading
    ? <LoadingView style={Style.array([|style, FormStyles.square(40.)|])} />
    : <IconButton
        size=40.
        icon=Icons.Refresh.build
        onPress={_ => onRefresh()}
        style
      />;
};
