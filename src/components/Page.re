open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "scroll": style(~flex=1., ()),
      "scrollContent":
        style(
          ~flex=1.,
          ~paddingVertical=LayoutConst.pagePaddingVertical->dp,
          ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~children) => {
  <DocumentContext.ScrollView
    style=styles##scroll contentContainerStyle=styles##scrollContent>
    children
  </DocumentContext.ScrollView>;
};
