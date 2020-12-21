open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "modal":
        style(~width=642.->dp, ~alignSelf=`center, ~borderRadius=4., ()),
    })
  );

let formStyles =
  Style.(
    StyleSheet.create({
      "modal":
        style(
          ~paddingTop=45.->dp,
          ~paddingBottom=32.->dp,
          ~paddingHorizontal=110.->dp,
          (),
        ),
    })
  );

module Base = {
  [@react.component]
  let make = (~children, ~style as styleFromProp=?) => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        arrayOption([|
          styleFromProp,
          Some(styles##modal),
          Some(style(~backgroundColor=theme.colors.background, ())),
        |])
      )>
      children
    </View>;
  };
};

module Form = {
  [@react.component]
  let make = (~children) => {
    <Base style=formStyles##modal> children </Base>;
  };
};

include Base;
