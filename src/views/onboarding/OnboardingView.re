open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`row,
          ~justifyContent=`spaceAround,
          ~paddingVertical=78.->dp,
          ~paddingHorizontal=58.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  <Page>
    <View style=styles##container>
      <CreateAccountBigButton />
      <ImportAccountBigButton />
    </View>
  </Page>;
};
