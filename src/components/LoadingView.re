open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(~flex=1., ~alignItems=`center, ~justifyContent=`center, ()),
    })
  );

[@react.component]
let make = () => {
  <View style=styles##container>
    <ActivityIndicator
      animating=true
      size=ActivityIndicator_Size.small
      color=Colors.highIcon
    />
  </View>;
};
