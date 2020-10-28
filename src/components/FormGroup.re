open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~flex=1., ~marginVertical=10.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~children,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  <View
    style=Style.(
      arrayOption([|
        Some(styles##formGroup),
        styleFromProp,
      |])
    )>
    children
  </View>;
};
