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

module Header = {
  let styles =
    Style.(
      StyleSheet.create({
        "header": style(~flexDirection=`row, ~marginBottom=10.->dp, ()),
        "actionRight":
          style(
            ~marginLeft=auto,
            ~marginRight=24.->dp,
            ~alignItems=`flexEnd,
            ~justifyContent=`flexStart,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children, ~right=?) => {
    <View style=styles##header>
      <View> children </View>
      {right->Option.mapWithDefault(React.null, right =>
         <View style=styles##actionRight> right </View>
       )}
    </View>;
  };
};
