open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "scroll": style(~flex=1., ()),
      "scrollContent":
        style(~paddingVertical=40.->dp, ~paddingHorizontal=36.->dp, ()),
    })
  );

[@react.component]
let make = (~children) => {
  <ScrollView style=styles##scroll contentContainerStyle=styles##scrollContent>
    children
  </ScrollView>;
};
