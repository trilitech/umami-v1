open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "switchCmp": style(~height=16.->dp, ~width=32.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
    })
  );

[@react.component]
let make = (~value: bool=false) => {
  let theme = ThemeContext.useTheme();

  <SwitchNative
    value
    thumbColor={theme.colors.background}
    trackColor={Switch.trackColor(
      ~_true=theme.colors.iconPrimary,
      ~_false=theme.colors.iconMediumEmphasis,
      (),
    )}
    style=styles##switchCmp
    thumbStyle=styles##switchThumb
  />;
};
