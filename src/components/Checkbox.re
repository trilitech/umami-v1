open ReactNative;

[@react.component]
let make = (~style: option(Style.t)=?, ~value: bool=false) => {
  let theme = ThemeContext.useTheme();
  let size = 24.;
  let color = {
    value ? theme.colors.iconHighEmphasis : theme.colors.iconMediumEmphasis;
  };
  value
    ? {
      <Icons.CheckboxSelected size color ?style />;
    }
    : {
      <Icons.CheckboxUnselected size color ?style />;
    };
};
