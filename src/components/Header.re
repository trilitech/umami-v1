open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~paddingRight=40.->dp,
          ~paddingLeft=NavBar.width->dp,
          (),
        ),
      "nameLogo": style(~flexDirection=`row, ~alignItems=`center, ()),
      "spacer": style(~width=14.->dp, ()),
      "darkModeButton": style(~alignSelf=`flexEnd, ()),
      "title": style(~fontFamily="CormorantGaramond", ~lineHeight=16., ()),
    })
  );

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();
  let switchTheme = ThemeContext.useSwitch();

  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )>
    <View style=styles##nameLogo>
      <SVGLogoTmp
        width={46.->Style.dp}
        height={46.->Style.dp}
        fill={theme.colors.iconHighEmphasis}
      />
      <View style=styles##spacer />
      <Typography.Base
        colorStyle=`highEmphasis
        fontSize=20.
        fontWeightStyle=`semiBold
        style=styles##title>
        {js|Umami|js}->React.string
      </Typography.Base>
    </View>
    <TouchableOpacity onPress={_ => switchTheme()}>
      <Icons.DarkMode size=36. color={theme.colors.iconDisabled} />
    </TouchableOpacity>
  </View>;
};
