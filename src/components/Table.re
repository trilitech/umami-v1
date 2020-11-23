open ReactNative;

module Head = {
  let styles =
    Style.(
      StyleSheet.create({
        "thead":
          style(
            ~flexDirection=`row,
            ~alignItems=`center,
            ~height=30.->dp,
            ~paddingLeft=22.->dp,
            ~borderColor="rgba(255,255,255,0.38)",
            ~borderBottomWidth=1.,
            ~zIndex=1,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children) => {
    <View style=styles##thead> children </View>;
  };
};

module Row = {
  let styles =
    Style.(StyleSheet.create({"borderSpacer": style(~width=20.->dp, ())}));

  [@react.component]
  let make = (~zIndex=?, ~children) => {
    <RowItem.Bordered height=48. style={Style.style(~zIndex?, ())}>
      <View style=styles##borderSpacer />
      children
    </RowItem.Bordered>;
  };
};

module Cell = {
  let styles =
    Style.(
      StyleSheet.create({
        "cell":
          style(~flexShrink=0., ~minWidth=75.->dp, ~marginRight=24.->dp, ()),
      })
    );

  [@react.component]
  let make = (~style as styleFromProp, ~children=?) => {
    <View style={Style.array([|styles##cell, styleFromProp|])} ?children />;
  };
};

module type StyleForCell = {let style: Style.t;};

module MakeCell = (CustomStyle: StyleForCell) => {
  [@react.component]
  let make = (~children=?) => {
    <Cell style=CustomStyle.style ?children />;
  };
};
