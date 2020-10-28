open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~width=28.->dp,
          ~height=28.->dp,
          ~marginRight=4.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~icon, ~onPress=?) => {
  <TouchableOpacity ?onPress>
    <View style=styles##button>
      <Icon name=icon size=16. color=Theme.colorDarkMediumEmphasis />
    </View>
  </TouchableOpacity>;
};
