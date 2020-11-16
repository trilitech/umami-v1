open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container": style(~marginVertical=10.->dp, ~maxHeight=400.->dp, ()),
    })
  );

[@react.component]
let make = (~error) => {
  <ScrollView style=styles##container alwaysBounceVertical=false>
    <Typography.Body3 colorStyle=`error>
      error->React.string
    </Typography.Body3>
  </ScrollView>;
};
