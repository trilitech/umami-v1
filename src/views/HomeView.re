open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "main": style(~padding=8.->dp, ~backgroundColor="#ffffff", ()),
      "header":
        style(
          ~flex=1.,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~margin=4.->dp,
          ~borderWidth=1.0,
          (),
        ),
      "section": style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()),
    })
  );

[@react.component]
let make = () => {
  let (href, onPress) = Routes.useHrefAndOnPress(Routes.Dev);

  <View>
    <Text> "HOME"->React.string </Text>
    <Text accessibilityRole=`link href onPress>
      "go to DEV"->React.string
    </Text>
  </View>;
};
