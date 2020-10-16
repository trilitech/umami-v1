open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~backgroundColor="#121212",
          (),
        ),
      "title":
        style(
          ~marginHorizontal=16.->dp,
          ~fontWeight=`bold,
          ~color="#FFF",
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  <View style=styles##container>
    <SVGLogo
      width={36.->Style.dp}
      height={38.->Style.dp}
      fill="#FFF"
      stroke="#FFF"
    />
    <Text style=styles##title> {js|Zebra\nWallet|js}->React.string </Text>
  </View>;
};
