/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNative
open Style
open Routes

let width = 110.

module NavBarItem = {
  let styles = {
    open Style
    StyleSheet.create({
      "item": style(
        ~marginVertical=9.->dp,
        ~minHeight=50.->dp,
        ~paddingVertical=3.->dp,
        ~alignItems=#center,
        ~justifyContent=#center,
        ~width=width->dp,
        (),
      ),
      "icon": style(~marginBottom=6.->dp, ()),
      "text": style(~textAlign=#center, ()),
    })
  }

  @react.component
  let make = (
    ~onPress,
    ~title,
    ~icon: option<Icons.builder>=?,
    ~iconSize: option<float>=?,
    ~isActive=false,
    ~showDot=false,
    ~colorStyle=?,
  ) => {
    let theme = ThemeContext.useTheme()

    let iconColor =
      colorStyle->Option.mapWithDefault(
        isActive
          ? theme.dark ? theme.colors.iconMaxEmphasis : theme.colors.iconPrimary
          : theme.colors.iconMediumEmphasis,
        colorStyle => colorStyle->Typography.getColor(theme),
      )

    let handlePress = a =>
      if !isActive {
        onPress(a)
      }

    <ThemedPressable accessibilityRole=#link onPress=handlePress style={styles["item"]} isActive>
      {icon->Option.mapWithDefault(React.null, icon =>
        <View>
          {icon(
            ~style=styles["icon"],
            ~size=iconSize->Option.getWithDefault(24.),
            ~color=iconColor,
          )}
          {showDot ? <Buttons.Dot /> : React.null}
        </View>
      )}
      <Typography.ButtonTernary
        style={styles["text"]}
        colorStyle=?{isActive ? Some(#highEmphasis) : colorStyle}
        fontWeightStyle=?{isActive ? Some(#black) : None}>
        {title->React.string}
      </Typography.ButtonTernary>
    </ThemedPressable>
  }
}

module NavBarItemRoute = {
  @react.component
  let make = (
    ~currentRoute,
    ~route,
    ~title,
    ~icon: option<Icons.builder>=?,
    ~iconSize: option<float>=?,
    ~showDot=false,
  ) => {
    let onPress = useHrefAndOnPress(route)
    let isCurrent = currentRoute == route

    <NavBarItem onPress title ?icon ?iconSize isActive=isCurrent showDot />
  }
}

module NavBarSmallItem = {
  @react.component
  let make = (~onPress, ~icon: Icons.builder, ~isActive=false, ~colorStyle=?) => {
    let theme = ThemeContext.useTheme()

    let iconColor =
      colorStyle->Option.mapWithDefault(
        isActive
          ? theme.dark ? theme.colors.iconMaxEmphasis : theme.colors.iconPrimary
          : theme.colors.iconMediumEmphasis,
        colorStyle => colorStyle->Typography.getColor(theme),
      )

    let handlePress = a =>
      if !isActive {
        onPress(a)
      }
    /* border-radius: (17 + 8 * 2) / 2 = 16.5 */
    <ThemedPressable
      accessibilityRole=#link
      onPress=handlePress
      style={ReactNative.Style.style(
        ~padding=8.->dp,
        ~overflow=#hidden,
        ~borderRadius=16.5,
        ~marginBottom=9.->dp,
        (),
      )}
      isActive>
      <View> {icon(~size=17., ~color=iconColor, ~style=ReactNative.Style.style())} </View>
    </ThemedPressable>
  }
}

module NavBarSmallItemRoute = {
  @react.component
  let make = (~currentRoute, ~route, ~icon: Icons.builder) => {
    let onPress = useHrefAndOnPress(route)
    let isCurrent = currentRoute == route

    <NavBarSmallItem onPress icon isActive=isCurrent />
  }
}

module LogsButton = {
  @react.component
  let make = (~currentRoute, ~route, ~icon: Icons.builder) => {
    let setSeen = LogsContext.useSetSeen()
    let seen = LogsContext.useSeen()
    let isCurrent = currentRoute == route
    let href = toHref(route)

    let onPress = event => {
      event->ReactNative.Event.PressEvent.preventDefault
      RescriptReactRouter.push(href)
      true->setSeen
    }

    <>
      <NavBarSmallItem onPress icon isActive=isCurrent colorStyle=?{!seen ? Some(#error) : None} />
    </>
  }
}

let styles = {
  open Style
  StyleSheet.create({
    "container": style(~flexDirection=#column, ~paddingBottom=15.->dp, ()),
    "sendButton": style(~marginTop=20.->dp, ~marginBottom=18.->dp, ~alignItems=#center, ()),
    "bottomContainer": style(
      ~marginTop=auto,
      ~display=#flex,
      ~flexDirection=#row,
      ~flexWrap=#wrap,
      ~alignContent=#center,
      ~justifyContent=#spaceEvenly,
      ~alignItems=#center,
      ~width=width->dp,
      (),
    ),
    "version": style(~display=#flex, ~justifyContent=#center, ()),
  })
}

module Empty = {
  @react.component
  let make = () => {
    let theme = ThemeContext.useTheme()
    <View
      style={
        open Style
        array([styles["container"], style(~backgroundColor=theme.colors.barBackground, ())])
      }
    />
  }
}

module NavBarItemOperations = {
  module Implicit = {
    @react.component
    let make = (~currentRoute) => {
      <NavBarItemRoute
        currentRoute route=Operations title=I18n.navbar_operations icon=Icons.History.build
      />
    }
  }

  module Multisig = {
    @react.component
    let make = (~address: PublicKeyHash.t, ~currentRoute) => {
      let hasPendingWaiting = StoreContext.useHasPendingWaiting()(address)
      <NavBarItemRoute
        currentRoute
        route=Operations
        title=I18n.navbar_operations
        icon=Icons.History.build
        showDot=hasPendingWaiting
      />
    }
  }
}

@react.component
let make = (~account: Alias.t, ~route as currentRoute) => {
  let theme = ThemeContext.useTheme()

  let {address} = account
  let batch = GlobalBatchContext.useGlobalBatchContext().batch(address)
  let hasBatchItems = batch != None

  <View
    style={
      open Style
      array([
        styles["container"],
        style(~backgroundColor=theme.colors.barBackground, ()),
        {
          open Style
          unsafeStyle({"overflow": "auto"})
        },

        /* No `auto value allowed for overflow prop */
      ])
    }>
    <View style={styles["sendButton"]}> <SendButton account /> </View>
    <NavBarItemRoute
      currentRoute route=Accounts title=I18n.navbar_accounts icon=Icons.Account.build
    />
    <NavBarItemRoute
      currentRoute route=Nft iconSize=28. title=I18n.navbar_nft icon=Icons.Nft.build
    />
    {PublicKeyHash.isImplicit(address)
      ? <NavBarItemOperations.Implicit currentRoute />
      : <NavBarItemOperations.Multisig currentRoute address />}
    <NavBarItemRoute
      currentRoute
      route=Batch
      title=I18n.navbar_global_batch
      icon=Icons.Batch.build
      showDot=hasBatchItems
    />
    <NavBarItemRoute
      currentRoute route=AddressBook title=I18n.navbar_addressbook icon=Icons.AddressBook.build
    />
    <NavBarItemRoute
      currentRoute route=Delegations title=I18n.navbar_delegations icon=Icons.Delegate.build
    />
    <NavBarItemRoute
      currentRoute route=Contracts title=I18n.navbar_contracts icon=Icons.Contract.build
    />
    <View style={styles["bottomContainer"]}>
      <NavBarSmallItemRoute currentRoute route=Settings icon=Icons.Settings.build />
      <NavBarSmallItemRoute currentRoute route=Help icon=Icons.Help.build />
      /* <NavBarItem currentRoute route=Debug title="DEBUG" /> */
      <LogsButton currentRoute route=Logs icon=Icons.Logs.build />
      <Typography.Overline3 style={styles["version"]}>
        {("v." ++ System.getVersion())->React.string}
      </Typography.Overline3>
    </View>
  </View>
}
