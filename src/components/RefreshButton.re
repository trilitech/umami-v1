open ReactNative;

[@react.component]
let make = (~onRefresh, ~loading, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  let style =
    Style.(arrayOption([|Some(SendButton.styles##button), styleArg|]));

  loading
    ? <LoadingView
        style={Style.array([|style, FormStyles.square(40.)|])}
        color={theme.colors.iconPrimary}
      />
    : <IconButton
        size=40.
        isActive=false
        icon={(~color as _=?) =>
          Icons.Refresh.build(~color=theme.colors.iconPrimary)
        }
        onPress={_ => onRefresh()}
        style
      />;
};
