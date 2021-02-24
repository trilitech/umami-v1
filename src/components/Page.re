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
  let onScroll = DocumentContext.useScrollListener();
  <ScrollView
    style=styles##scroll
    contentContainerStyle=styles##scrollContent
    onScroll
    scrollEventThrottle=250>
    children
  </ScrollView>;
};
