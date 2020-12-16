open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flexDirection=`row, ~height=60.->dp, ~alignItems=`center, ()),
      "spacer": style(~width=16.->dp, ()),
      "title": style(~fontFamily="CormorantGaramond", ~lineHeight=16., ()),
    })
  );

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();
  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )>
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
  </View>;
};
