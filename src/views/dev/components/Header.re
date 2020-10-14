open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`row,
          ~height=60.->dp,
          ~alignItems=`center,
          ~paddingHorizontal=22.->dp,
          ~backgroundColor="#121212",
          (),
        ),
      "title": style(~fontWeight=`bold, ~color="#FFF", ()),
    })
  );

[@react.component]
let make = () => {
  <View style=styles##container>
    <Text style=styles##title> {js|Zebra\nwallet|js}->React.string </Text>
  </View>;
};
