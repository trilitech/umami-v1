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

let styles =
  Style.(
    StyleSheet.create({
      "layout": style(~flex=1., ~flexDirection=`column, ()),
      "main": style(~flex=1., ~flexDirection=`row, ()),
      "content": style(~flex=1., ()),
    })
  );

module EmptyAppView = {
  [@react.component]
  let make = () => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        array([|
          styles##layout,
          style(~backgroundColor=theme.colors.background, ()),
        |])
      )>
      <Header />
      <View style=styles##main>
        <NavBar.Empty />
        <View style=styles##content />
      </View>
    </View>;
  };
};

let menu = {
  open System.Menu;
  let appMenuRole = System.isMac ? `appMenu : `fileMenu;

  let supportUrl = "https://umamiwallet.com/#support";
  let downloadUrl = "https://umamiwallet.com/#download";
  let websiteUrl = "https://umamiwallet.com";

  let supportItem =
    mkItem(
      ~label=I18n.menu#app_menu_support,
      ~click=_ => System.openExternal(supportUrl),
      (),
    );
  let downloadItem =
    mkItem(
      ~label=I18n.menu#app_menu_new_version,
      ~click=_ => System.openExternal(downloadUrl),
      (),
    );
  let websiteItem =
    mkItem(
      ~label=I18n.menu#app_menu_website,
      ~click=_ => System.openExternal(websiteUrl),
      (),
    );

  buildFromTemplate([|
    mkItem(~role=appMenuRole, ()),
    mkItem(~role=`editMenu, ()),
    mkItem(~role=`viewMenu, ()),
    mkSubmenu(
      ~label=I18n.menu#app_menu_help,
      ~submenu=[|supportItem, downloadItem, websiteItem|],
      (),
    ),
  |]);
};

module AppView = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    let route = Routes.match(url);

    let accounts = StoreContext.Accounts.useGetAll();
    let accountsRequest = StoreContext.Accounts.useRequest();

    let (displayOnboarding, displayNavbar) = {
      switch (accountsRequest) {
      | Done(_) when accounts->Map.String.size <= 0 => (true, false)
      | NotAsked => (false, false)
      | Loading(None) => (false, false)
      | Loading(Some(_)) => (false, true)
      | Done(_) => (false, true)
      };
    };

    let theme = ThemeContext.useTheme();

    React.useEffect0(() => {
      System.Menu.setApplicationMenu(menu);
      None;
    });

    <DocumentContext>
      <View
        style=Style.(
          array([|
            styles##layout,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )>
        <Header />
        <View style=styles##main>
          {displayNavbar ? <NavBar route /> : <NavBar.Empty />}
          <View style=styles##content>
            {displayOnboarding
               ? <OnboardingView />
               : (
                 switch (route) {
                 | Accounts => <AccountsView />
                 | Operations => <OperationsView />
                 | AddressBook => <AddressBookView />
                 | Delegations => <DelegationsView />
                 | Tokens => <TokensView />
                 | Settings => <SettingsView />
                 | NotFound =>
                   <View>
                     <Typography.Body1>
                       I18n.t#error404->React.string
                     </Typography.Body1>
                   </View>
                 }
               )}
          </View>
        </View>
      </View>
    </DocumentContext>;
  };
};

[@react.component]
let make = () => {
  <LogsContext>
    <ConfigContext>
      <ThemeContext>
        <SdkContext empty={() => <EmptyAppView />}>
          <StoreContext> <AppView /> </StoreContext>
        </SdkContext>
      </ThemeContext>
    </ConfigContext>
  </LogsContext>;
};
