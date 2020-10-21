open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flex=1., ~alignItems=`center, ~justifyContent=`center, ()),
      "text": style(~color="#FFF", ()),
    })
  );

[@react.component]
let make = (~text) => {
  <View style=styles##container>
    <Text style=styles##text> text->React.string </Text>
  </View>;
};
