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
            ~borderBottomWidth=1.,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~children) => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        array([|
          styles##thead,
          style(~borderColor=theme.colors.borderDisabled, ()),
        |])
      )>
      children
    </View>;
  };
};

module Row = {
  let styles =
    Style.(StyleSheet.create({"borderSpacer": style(~width=20.->dp, ())}));

  [@react.component]
  let make = (~children) => {
    <RowItem.Bordered height=48.>
      <View style=styles##borderSpacer />
      children
    </RowItem.Bordered>;
  };
};

module Empty = {
  let styles =
    Style.(
      StyleSheet.create({
        "empty": style(~paddingLeft=22.->dp, ~paddingTop=14.->dp, ()),
      })
    );

  [@react.component]
  let make = (~children) => {
    <Typography.Body1 style=styles##empty> children </Typography.Body1>;
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
  let make =
      (
        ~style as styleFromProp: ReactNative.Style.t,
        ~children: option(React.element)=?,
      ) => {
    <View style={Style.array([|styles##cell, styleFromProp|])} ?children />;
  };
};

module type StyleForCell = {let style: Style.t;};

module MakeCell = (CustomStyle: StyleForCell) => {
  let makeProps = Cell.makeProps(~style=CustomStyle.style);
  let make = Cell.make;
};
