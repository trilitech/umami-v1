open ReactNative;

module Radio = {
  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~setValue: (ThemeContext.themeMain => ThemeContext.themeMain) => unit,
        ~currentValue,
      ) => {
    <TouchableOpacity onPress={_ => setValue(_ => value)}>
      <Typography.Body1
        fontWeightStyle=?{currentValue == value ? Some(`bold) : None}>
        label->React.string
      </Typography.Body1>
    </TouchableOpacity>;
  };
};

[@react.component]
let make = () => {
  let (themeSetting, setThemeSetting) = ThemeContext.useThemeSetting();

  <Page>
    <Typography.Overline1> "Theme"->React.string </Typography.Overline1>
    <Radio
      label="Light"
      value=`light
      setValue=setThemeSetting
      currentValue=themeSetting
    />
    <Radio
      label="Dark"
      value=`dark
      setValue=setThemeSetting
      currentValue=themeSetting
    />
  </Page>;
};
