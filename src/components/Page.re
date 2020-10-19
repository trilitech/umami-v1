open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "scroll": style(~flex=1., ()),
      "scrollContent":
        style(
          ~paddingVertical=Theme.pagePaddingVertical->dp,
          ~paddingHorizontal=Theme.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~children) => {
  <ScrollView style=styles##scroll contentContainerStyle=styles##scrollContent>
    children
  </ScrollView>;
};
