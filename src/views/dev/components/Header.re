open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~backgroundColor=Colors.structBackground,
          (),
        ),
      "spacer": style(~width=16.->dp, ()),
      "title": style(~lineHeight=16., ()),
    })
  );

[@react.component]
let make = () => {
  <View style=styles##container>
    <SVGLogo
      width={36.->Style.dp}
      height={38.->Style.dp}
      fill=Colors.highIcon
      stroke=Colors.highIcon
    />
    <View style=styles##spacer />
    <Typography.Base
      colorStyle=`highEmphasis
      fontSize=14.
      fontWeightStyle=`black
      style=styles##title>
      {js|Zebra\nWallet|js}->React.string
    </Typography.Base>
  </View>;
};
