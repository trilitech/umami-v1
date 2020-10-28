open ReactNative;
open Routes;

module NavBarItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~minHeight=68.->dp,
            ~paddingVertical=12.->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            (),
          ),
        "icon": style(~marginBottom=6.->dp, ()),
        "text": style(~textAlign=`center, ()),
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
      <Typography.ButtonPrimary
        style=styles##text
        colorStyle={isCurrent ? `highEmphasis : `disabled}
        fontSize=10.>
        title->React.string
      </Typography.ButtonPrimary>
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
          ~backgroundColor=Colors.structBackground,
          (),
        ),
      "sendButton": style(~marginTop=20.->dp, ~marginBottom=18.->dp, ()),
    })
  );

[@react.component]
let make = (~route as currentRoute) => {
  <View style=styles##container>
    <View style=styles##sendButton> <SendButton /> </View>
    <NavBarItem currentRoute route=Accounts title="ACCOUNTS" icon=`accounts />
    <NavBarItem
      currentRoute
      route=Operations
      title="OPERATIONS"
      icon=`history
    />
    <NavBarItem
      currentRoute
      route=AddressBook
      title={js|ADDRESS\nBOOK|js}
      icon=`addressBook
    />
    <NavBarItem currentRoute route=Debug title="DEBUG" />
  </View>;
};
