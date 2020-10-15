open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "modal":
        style(
          ~width=642.->dp,
          ~alignSelf=`center,
          ~paddingTop=45.->dp,
          ~paddingBottom=32.->dp,
          ~paddingHorizontal=110.->dp,
          ~backgroundColor="#121212",
          ~borderRadius=4.,
          (),
        ),
    })
  );

[@react.component]
let make = (~children) => {
  <View style=styles##modal> children </View>;
};
