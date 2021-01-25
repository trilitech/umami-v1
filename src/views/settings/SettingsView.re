open ReactNative;

module RadioItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container":
          style(
            ~alignSelf=`flexStart,
            ~marginLeft=24.->dp,
            ~height=37.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            (),
          ),
        "radio": style(~padding=8.->dp, ~marginRight=8.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~setValue: (ThemeContext.themeMain => ThemeContext.themeMain) => unit,
        ~currentValue,
      ) => {
    <TouchableOpacity
      style=styles##container onPress={_ => setValue(_ => value)}>
      <Radio value={currentValue == value} style=styles##radio />
      <Typography.Body1> label->React.string </Typography.Body1>
    </TouchableOpacity>;
  };
};

module Bloc = {
  let styles =
    Style.(
      StyleSheet.create({
        "bloc":
          style(
            ~paddingBottom=32.->dp,
            ~marginBottom=16.->dp,
            ~borderBottomWidth=1.,
            (),
          ),
        "title": style(~marginLeft=16.->dp, ~marginBottom=18.->dp, ()),
      })
    );

  [@react.component]
  let make = (~title, ~children) => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        array([|
          styles##bloc,
          style(~borderBottomColor=theme.colors.borderDisabled, ()),
        |])
      )>
      <Typography.Headline fontSize=16. style=styles##title>
        title->React.string
      </Typography.Headline>
      children
    </View>;
  };
};

[@react.component]
let make = () => {
  let (themeSetting, setThemeSetting) = ThemeContext.useThemeSetting();

  <Page>
    <Bloc title=I18n.settings#theme_title>
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
    </Bloc>
  </Page>;
};
