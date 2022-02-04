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

module DisclaimerModal = {
  [@react.component]
  let make = (~onSign) => {
    <Page>
      <View style=Style.(style(~paddingTop=27.->dp, ~flex=1., ()))>
        <DisclaimerView onSign />
      </View>
    </Page>;
  };
};

module Homepage = {
  type state =
    | Onboarding
    | BuyTez(string)
    | AddAccountModal
    | Dashboard;
};

module BuyTezView = {
  module IFrame = {
    type props = {
      src: string,
      width: string,
      height: string,
      allow: string,
      frameBorder: string,
    };

    [@bs.val] [@bs.scope "React"]
    external createElement: ([@bs.as "iframe"] _, props) => React.element =
      "createElement";

    [@react.component]
    let make = (~src) => {
      createElement({
        src,
        width: "100%",
        height: "100%",
        allow: "camera *; microphone *",
        frameBorder: "0",
      });
    };
  };

  [@react.component]
  let make = (~src, ~onClose) => {
    <Page>
      <Page.Header
        left={
          <Typography.Body1
            colorStyle=`error
            style=Style.(style(~marginTop=10.->dp, ~textAlign=`center, ()))>
            I18n.Expl.external_service->React.string
          </Typography.Body1>
        }
        right={<CloseButton onClose />}>
        ReasonReact.null
      </Page.Header>
      <IFrame src />
    </Page>;
  };
};

module Dashboard = {
  [@react.component]
  let make = (~route: Routes.t, ~showBuyTez, ~setMainPage) => {
    let (accountsViewMode, setAccountsViewMode) =
      React.useState(_ => AccountsView.Mode.Simple);

    <>
      {switch (route) {
       | Accounts =>
         <AccountsView
           mode=accountsViewMode
           setMode=setAccountsViewMode
           showBuyTez
           showOnboarding={() => setMainPage(_ => Homepage.AddAccountModal)}
         />
       | Nft => <NftView />
       | Operations => <OperationsView />
       | AddressBook => <AddressBookView />
       | Delegations => <DelegationsView />
       | Tokens => <TokensView />
       | Settings => <SettingsView />
       | Logs => <LogsView />
       | NotFound =>
         <View>
           <Typography.Body1> I18n.error404->React.string </Typography.Body1>
         </View>
       }}
    </>;
  };
};

let shouldDisplayNavbar =
    (
      ~accountsRequest:
         Umami.ApiRequest.t(Umami.PublicKeyHash.Map.map(Umami.Account.t)),
      ~accounts,
      ~mainPageState: Homepage.state,
    ) =>
  switch (accountsRequest) {
  | Done(_) when accounts->PublicKeyHash.Map.size <= 0 => false
  | NotAsked => false
  | Loading(None) => false
  | Loading(Some(_)) => true
  | Done(_) =>
    switch (mainPageState) {
    | Onboarding
    | AddAccountModal => false
    | BuyTez(_) => false
    | Dashboard => true
    }
  };

module AppView = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    let route = Routes.match(url);

    let accounts = StoreContext.Accounts.useGetAll();
    let accountsRequest = StoreContext.Accounts.useRequest();
    let eulaSignature = StoreContext.useEulaSignature();
    let setEulaSignature = StoreContext.setEulaSignature();

    let onSign = needSign => setEulaSignature(_ => needSign);

    let (mainPageState, setMainPage) =
      React.useState(_ => Homepage.Dashboard);

    React.useLayoutEffect1(
      () =>
        switch (accountsRequest) {
        | Done(_) when accounts->PublicKeyHash.Map.size <= 0 =>
          setMainPage(_ => Onboarding);
          None;
        | _ =>
          setMainPage(_ => Dashboard);
          None;
        },
      [|accountsRequest|],
    );

    let theme = ThemeContext.useTheme();

    let displayNavbar =
      shouldDisplayNavbar(~accountsRequest, ~accounts, ~mainPageState);

    let handleCloseBuyTezView = _ => {
      setMainPage(_ => Dashboard);
    };

    <DocumentContext>
      <View
        style=Style.(
          array([|
            styles##layout,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )>
        <Header />
        {eulaSignature
           ? <DisclaimerModal onSign />
           : <View style=styles##main>
               {displayNavbar ? <NavBar route /> : <NavBar.Empty />}
               <View style=styles##content>
                 {switch (mainPageState) {
                  | Onboarding => <OnboardingView />
                  | AddAccountModal =>
                    <OnboardingView
                      onClose={_ => setMainPage(_ => Dashboard)}
                    />
                  | BuyTez(src) =>
                    <BuyTezView src onClose=handleCloseBuyTezView />
                  | Dashboard =>
                    <Dashboard
                      showBuyTez={url => setMainPage(_ => BuyTez(url))}
                      route
                      setMainPage
                    />
                  }}
               </View>
             </View>}
      </View>
    </DocumentContext>;
  };
};

[@react.component]
let make = () => {
  <LogsContext>
    <ConfigFileContext>
      <ConfigContext>
        <ThemeContext>
          <StoreContext> <AppView /> <BeaconConnectRequest /> </StoreContext>
        </ThemeContext>
      </ConfigContext>
    </ConfigFileContext>
  </LogsContext>;
};
