open ReactNative;

[@react.component]
let make = (~style: option(Style.t)=?, ~value: bool=false) => {
  let theme = ThemeContext.useTheme();
  let size = 20.;
  let color = {
    value ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;
  };
  value
    ? {
      <Icons.CheckboxSelected size color ?style />;
    }
    : {
      <Icons.CheckboxUnselected size color ?style />;
    };
};
