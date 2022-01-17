/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

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
  let make =
      (
        ~onPress,
        ~title,
        ~icon: option(Icons.builder)=?,
        ~iconSize: option(float)=?,
        ~isActive=false,
        ~colorStyle=?,
      ) => {
    let theme = ThemeContext.useTheme();

    let iconColor =
      colorStyle->Option.mapWithDefault(
        isActive
          ? theme.dark
              ? theme.colors.iconMaxEmphasis : theme.colors.iconPrimary
          : theme.colors.iconMediumEmphasis,
        colorStyle =>
        colorStyle->Typography.getColor(theme)
      );

    let handlePress = a =>
      if (!isActive) {
        onPress(a);
      };

    <ThemedPressable
      accessibilityRole=`link onPress=handlePress style=styles##item isActive>
      {icon->Option.mapWithDefault(React.null, icon => {
         icon(
           ~style={
             styles##icon;
           },
           ~size=iconSize->Option.getWithDefault(24.),
           ~color=iconColor,
         )
       })}
      <Typography.ButtonTernary
        style=styles##text
        colorStyle=?{isActive ? Some(`highEmphasis) : colorStyle}
        fontWeightStyle=?{isActive ? Some(`black) : None}>
        title->React.string
      </Typography.ButtonTernary>
    </ThemedPressable>;
  };
};

module NavBarItemRoute = {
  [@react.component]
  let make =
      (
        ~currentRoute,
        ~route,
        ~title,
        ~icon: option(Icons.builder)=?,
        ~iconSize: option(float)=?,
      ) => {
    let onPress = useHrefAndOnPress(route);
    let (href, onPress) = useHrefAndOnPress(route);
    let isCurrent = currentRoute == route;

    <NavBarItem onPress title ?icon ?iconSize isActive=isCurrent />;
  };
};

module LogsButton = {
  [@react.component]
  let make = (~currentRoute, ~route, ~title, ~icon: option(Icons.builder)=?) => {
    let setSeen = LogsContext.useSetSeen();
    let seen = LogsContext.useSeen();
    let isCurrent = currentRoute == route;
    let href = toHref(route);

    let onPress = event => {
      event->ReactNative.Event.PressEvent.preventDefault;
      ReasonReactRouter.push(href);
      true->setSeen;
    };

    <>
      <NavBarItem
        onPress
        title
        ?icon
        isActive=isCurrent
        colorStyle=?{!seen ? Some(`error) : None}
      />
    </>;
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

module Empty = {
  [@react.component]
  let make = () => {
    let theme = ThemeContext.useTheme();
    <View
      style=Style.(
        array([|
          styles##container,
          style(~backgroundColor=theme.colors.barBackground, ()),
        |])
      )
    />;
  };
};

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
    <NavBarItemRoute
      currentRoute
      route=Accounts
      title=I18n.navbar_accounts
      icon=Icons.Account.build
    />
    <NavBarItemRoute
      currentRoute
      route=Nft
      iconSize=28.
      title=I18n.navbar_nft
      icon=Icons.Nft.build
    />
    <NavBarItemRoute
      currentRoute
      route=Operations
      title=I18n.navbar_operations
      icon=Icons.History.build
    />
    <NavBarItemRoute
      currentRoute
      route=AddressBook
      title=I18n.navbar_addressbook
      icon=Icons.AddressBook.build
    />
    <NavBarItemRoute
      currentRoute
      route=Delegations
      title=I18n.navbar_delegations
      icon=Icons.Delegate.build
    />
    <NavBarItemRoute
      currentRoute
      route=Tokens
      title=I18n.navbar_tokens
      icon=Icons.Token.build
    />
    <NavBarItemRoute
      currentRoute
      route=Settings
      title=I18n.navbar_settings
      icon=Icons.Settings.build
    />
    /* <NavBarItem currentRoute route=Debug title="DEBUG" /> */
    <View style=styles##bottomContainer>
      <LogsButton
        currentRoute
        route=Logs
        title=I18n.navbar_logs
        icon=Icons.Logs.build
      />
      <Typography.Overline3 style=styles##version>
        {("v." ++ System.getVersion())->React.string}
      </Typography.Overline3>
    </View>
  </View>;
};
