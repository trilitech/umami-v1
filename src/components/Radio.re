open ReactNative;

[@react.component]
let make = (~style: option(Style.t)=?, ~value: bool=false) => {
  let theme = ThemeContext.useTheme();
  let size = 16.;
  let color = {
    value ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;
  };
  value
    ? {
      <Icons.RadioOn size color ?style />;
    }
    : {
      <Icons.RadioOff size color ?style />;
    };
};
