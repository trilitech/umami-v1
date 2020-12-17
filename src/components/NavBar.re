open ReactNative;
open Routes;

module NavBarItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "item":
          style(
            ~marginVertical=9.->dp,
            ~minHeight=50.->dp,
            ~paddingVertical=3.->dp,
            ~alignItems=`center,
            ~justifyContent=`center,
            (),
          ),
        "icon": style(~marginBottom=6.->dp, ()),
        "text": style(~textAlign=`center, ()),
        "errorButton": style(~marginTop=auto, ()),
      })
    );

  [@react.component]
  let make = (~currentRoute, ~route, ~title, ~icon: option(Icons.builder)=?) => {
    let theme = ThemeContext.useTheme();

    let (href, onPress) = useHrefAndOnPress(route);

    let account = StoreContext.SelectedAccount.useGet();
    let onPress = account->Belt.Option.map(_ => onPress);
    let href = account->Belt.Option.map(_ => href);
    let currentRoute = account->Belt.Option.map(_ => currentRoute);

    let isCurrent = currentRoute == Some(route);

    <ThemedPressable
      accessibilityRole=`link
      ?href
      ?onPress
      style=styles##item
      isActive=isCurrent>
      {icon->Belt.Option.mapWithDefault(React.null, icon => {
         icon(
           ~style={
             styles##icon;
           },
           ~size=24.,
           ~color={
             isCurrent
               ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis;
           },
         )
       })}
      <Typography.ButtonPrimary
        style=styles##text
        colorStyle={isCurrent ? `highEmphasis : `mediumEmphasis}
        fontSize=10.>
        title->React.string
      </Typography.ButtonPrimary>
    </ThemedPressable>;
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
          (),
        ),
      "sendButton":
        style(
          ~marginTop=20.->dp,
          ~marginBottom=18.->dp,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~route as currentRoute) => {
  let theme = ThemeContext.useTheme();
  <View
    style=Style.(
      array([|
        styles##container,
        style(~backgroundColor=theme.colors.barBackground, ()),
      |])
    )>
    <View style=styles##sendButton> <SendButton /> </View>
    <NavBarItem
      currentRoute
      route=Accounts
      title=I18n.t#navbar_accounts
      icon=Icons.Account.build
    />
    <NavBarItem
      currentRoute
      route=Operations
      title=I18n.t#navbar_operations
      icon=Icons.History.build
    />
    <NavBarItem
      currentRoute
      route=AddressBook
      title=I18n.t#navbar_addressbook
      icon=Icons.AddressBook.build
    />
    <NavBarItem
      currentRoute
      route=Delegations
      title=I18n.t#navbar_delegations
      icon=Icons.Delegate.build
    />
    <NavBarItem
      currentRoute
      route=Tokens
      title=I18n.t#navbar_tokens
      icon=Icons.Token.build
    />
    <NavBarItem currentRoute route=Debug title="DEBUG" />
    <LogsButton
      style=Style.(
        array([|NavBarItem.styles##errorButton, NavBarItem.styles##item|])
      )
    />
  </View>;
};
