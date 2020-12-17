open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flex=1., ~alignItems=`center, ~justifyContent=`center, ()),
    })
  );

[@react.component]
let make = (~text) => {
  <View style=styles##container>
    <Typography.Body2> text->React.string </Typography.Body2>
  </View>;
};
