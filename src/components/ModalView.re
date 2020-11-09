open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "modal":
        style(
          ~width=642.->dp,
          ~alignSelf=`center,
          ~backgroundColor=Colors.structBackground,
          ~borderRadius=4.,
          (),
        ),
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
  let make = (~children, ~style=?) => {
    <View style={Style.arrayOption([|style, Some(styles##modal)|])}>
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
