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
      })
    );

  [@react.component]
  let make = (~currentRoute, ~route, ~title, ~icon: option(Icons.builder)=?) => {
    let theme = ThemeContext.useTheme();

    let (href, onPress) = useHrefAndOnPress(route);

    let isCurrent = currentRoute == route;

    <ThemedPressable
      accessibilityRole=`link
      href
      onPress
      style=styles##item
      isActive=isCurrent>
      {icon->Option.mapWithDefault(React.null, icon => {
         icon(
           ~style={
             styles##icon;
           },
           ~size=24.,
           ~color={
             isCurrent
               ? theme.dark
                   ? theme.colors.iconMaxEmphasis : theme.colors.iconPrimary
               : theme.colors.iconMediumEmphasis;
           },
         )
       })}
      <Typography.ButtonTernary
        style=styles##text
        colorStyle=?{isCurrent ? Some(`highEmphasis) : None}
        fontWeightStyle=?{isCurrent ? Some(`black) : None}>
        title->React.string
      </Typography.ButtonTernary>
    </ThemedPressable>;
  };
};

let width = 110.;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~flexDirection=`column,
          ~width=width->dp,
          ~paddingBottom=15.->dp,
          (),
        ),
      "sendButton":
        style(
          ~marginTop=20.->dp,
          ~marginBottom=18.->dp,
          ~alignItems=`center,
          (),
        ),
      "bottomContainer": style(~marginTop=auto, ()),
      "version": style(~display=`flex, ~justifyContent=`center, ()),
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
    <NavBarItem
      currentRoute
      route=Settings
      title=I18n.t#navbar_settings
      icon=Icons.Settings.build
    />
    /* <NavBarItem currentRoute route=Debug title="DEBUG" /> */
    <View style=styles##bottomContainer>
      <LogsButton style=NavBarItem.styles##item />
      <Typography.Overline3 style=styles##version>
        {("v." ++ System.getVersion())->React.string}
      </Typography.Overline3>
    </View>
  </View>;
};
