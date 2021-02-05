open SettingsComponents;

[@react.component]
let make = () => {
  let (themeSetting, setThemeSetting) = ThemeContext.useThemeSetting();

  <Block title=I18n.settings#theme_title>
    <ColumnLeft>
      <RadioItem
        label=I18n.settings#theme_system
        value=`system
        setValue=setThemeSetting
        currentValue=themeSetting
      />
      <RadioItem
        label=I18n.settings#theme_dark
        value=`dark
        setValue=setThemeSetting
        currentValue=themeSetting
      />
      <RadioItem
        label=I18n.settings#theme_light
        value=`light
        setValue=setThemeSetting
        currentValue=themeSetting
      />
    </ColumnLeft>
  </Block>;
};
