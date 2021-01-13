open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "scrim":
        StyleSheet.flatten([|
          StyleSheet.absoluteFillObject,
          style(
            ~flexDirection=`row,
            ~justifyContent=`spaceAround,
            ~paddingVertical=78.->dp,
            ~paddingHorizontal=58.->dp,
            (),
          ),
        |]),
    })
  );

[@react.component]
let make = () => {
  let theme = ThemeContext.useTheme();

  <Page>
    <View
      style=Style.(
        array([|
          styles##scrim,
          style(~backgroundColor=theme.colors.scrim, ()),
        |])
      )>
      <CreateAccountBigButton />
      <ImportAccountBigButton />
    </View>
  </Page>;
};
