open ReactNative;

let styles =
  Style.(
    StyleSheet.create({"container": style(~marginVertical=10.->dp, ())})
  );

[@react.component]
let make = (~error) => {
  <View style=styles##container>
    <Typography.Body3 colorStyle=`error>
      error->React.string
    </Typography.Body3>
  </View>;
};
