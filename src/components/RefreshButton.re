open ReactNative;

[@react.component]
let make = (~onRefresh, ~style as styleArg=?) => {
  let theme = ThemeContext.useTheme();

  <IconButton
    size=40.
    icon={(~color as _=?) =>
      Icons.Refresh.build(~color=theme.colors.iconPrimary)
    }
    onPress={_ => onRefresh()}
    style=Style.(arrayOption([|Some(SendButton.styles##button), styleArg|]))
  />;
};
