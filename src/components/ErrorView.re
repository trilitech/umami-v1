open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flex=1., ~alignItems=`center, ~justifyContent=`center, ()),
    })
  );

[@react.component]
let make = (~error) => {
  <View style=styles##container>
    <Typography.Body3 colorStyle=`error>
      error->React.string
    </Typography.Body3>
  </View>;
};
