open ReactNative;
open Routes;

module NavBarItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~height=64.->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            (),
          ),
        "text": style(~color="white", ()),
      })
    );

  [@react.component]
  let make = (~route, ~title, ~icon=?) => {
    let (href, onPress) = useHrefAndOnPress(route);

    <TouchableOpacity style=styles##item accessibilityRole=`link href onPress>
      {icon->Belt.Option.mapWithDefault(React.null, name =>
         <Icon name size=24. color="#FFF" />
       )}
      <Text style=styles##text> title->React.string </Text>
    </TouchableOpacity>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`column,
          ~width=110.->dp,
          ~paddingTop=60.->dp,
          ~backgroundColor="#121212",
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  <View style={styles##container}>
    <NavBarItem route=Home title="Home" icon=`home />
    <NavBarItem route=Operations title="Operations" icon=`history />
    <NavBarItem route=Debug title="Debug" />
  </View>;
};
