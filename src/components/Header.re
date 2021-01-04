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
          (),
        ),
      "nameLogo": style(~flexDirection=`row, ~alignItems=`center, ()),
      "spacer": style(~width=16.->dp, ()),
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
      <SVGLogo
        width={36.->Style.dp}
        height={38.->Style.dp}
        fill={theme.colors.iconHighEmphasis}
        stroke={theme.colors.iconHighEmphasis}
      />
      <View style=styles##spacer />
      <Typography.Base
        colorStyle=`highEmphasis
        fontSize=20.
        fontWeightStyle=`semiBold
        style=styles##title>
        {js|Zebra Wallet|js}->React.string
      </Typography.Base>
    </View>
    <TouchableOpacity onPress={_ => switchTheme()}>
      <Icons.DarkMode size=36. color={theme.colors.iconDisabled} />
    </TouchableOpacity>
  </View>;
};
