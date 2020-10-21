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
            (),
          ),
        "icon": style(~marginBottom=6.->dp, ()),
      })
    );

  [@react.component]
  let make = (~currentRoute, ~route, ~title, ~icon=?) => {
    let (href, onPress) = useHrefAndOnPress(route);

    let isCurrent = currentRoute == route;

    <TouchableOpacity style=styles##item accessibilityRole=`link href onPress>
      {icon->Belt.Option.mapWithDefault(React.null, name =>
         <Icon
           name
           size=24.
           color={
             isCurrent ? Theme.colorDarkHighEmphasis : Theme.colorDarkDisabled
           }
           style=styles##icon
         />
       )}
      <Typography.ButtonPrimary10
        colorStyle={isCurrent ? `highEmphasis : `disabled}>
        title->React.string
      </Typography.ButtonPrimary10>
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
          ~backgroundColor=Colors.structBackground,
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
