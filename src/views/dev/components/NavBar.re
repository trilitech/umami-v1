open ReactNative;
open Routes;

module NavBarItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~height=68.->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            ~opacity=0.6,
            (),
          ),
        "itemCurrent": style(~opacity=1., ()),
        "icon": style(~marginBottom=6.->dp, ()),
        "text": style(~color="white", ~fontSize=10., ~fontWeight=`_700, ()),
      })
    );

  [@react.component]
  let make = (~currentRoute, ~route, ~title, ~icon=?) => {
    let (href, onPress) = useHrefAndOnPress(route);

    <TouchableOpacity
      style=Style.(
        arrayOption([|
          Some(styles##item),
          currentRoute == route ? Some(styles##itemCurrent) : None,
        |])
      )
      accessibilityRole=`link
      href
      onPress>
      {icon->Belt.Option.mapWithDefault(React.null, name =>
         <Icon name size=24. color="#FFF" style=styles##icon />
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
          ~paddingTop=(60. +. 10.)->dp,
          ~backgroundColor="#121212",
          (),
        ),
    })
  );

[@react.component]
let make = (~route as currentRoute) => {
  <View style={styles##container}>
    <NavBarItem currentRoute route=Home title="HOME" icon=`home />
    <NavBarItem
      currentRoute
      route=Operations
      title="OPERATIONS"
      icon=`history
    />
    <NavBarItem currentRoute route=Debug title="DEBUG" />
  </View>;
};
