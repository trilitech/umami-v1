open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~flex=1., ~marginVertical=10.->dp, ()),
      "formGroupSmall": style(~marginVertical=7.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~children,
      ~small=false,
      ~style as styleFromProp: option(ReactNative.Style.t)=?,
    ) => {
  <View
    style=Style.(
      arrayOption([|
        Some(styles##formGroup),
        small ? Some(styles##formGroupSmall) : None,
        styleFromProp,
      |])
    )>
    children
  </View>;
};
