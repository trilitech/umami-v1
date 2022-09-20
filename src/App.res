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

let styles = {
  open Style
  StyleSheet.create({
    "layout": style(~flex=1., ~flexDirection=#column, ()),
    "main": style(~flex=1., ~flexDirection=#row, ()),
    "content": style(~flex=1., ()),
  })
}

module DisclaimerModal = {
  @react.component
  let make = (~onSign) =>
    <Page>
      <View
        style={
          open Style
          style(~paddingTop=27.->dp, ~flex=1., ())
        }>
        <DisclaimerView onSign />
      </View>
    </Page>
}

module Homepage = {
  type state =
    | Onboarding
    | BuyTez(string)
    | AddAccountModal
    | Dashboard
}

module BuyTezView = {
  module IFrame = {
    type props = {
      src: string,
      width: string,
      height: string,
      allow: string,
      frameBorder: string,
    }

    @val @scope("React")
    external createElement: (@as("iframe") _, props) => React.element = "createElement"

    @react.component
    let make = (~src) =>
      createElement({
        src: src,
        width: "100%",
        height: "100%",
        allow: "camera *; microphone *",
        frameBorder: "0",
      })
  }

  @react.component
  let make = (~src, ~onClose) =>
    <Page>
      <Page.Header
        left={<Typography.Body1
          colorStyle=#error
          style={
            open Style
            style(~marginTop=10.->dp, ~textAlign=#center, ())
          }>
          {I18n.Expl.external_service->React.string}
        </Typography.Body1>}
        right={<CloseButton onClose />}>
        ReasonReact.null
      </Page.Header>
      <IFrame src />
    </Page>
}

module SelectedAccountView = {
  @react.component
  let make = (~children) => {
    let selectedAccount = StoreContext.SelectedAccount.useGetAtInit()

    selectedAccount->ReactUtils.mapOpt(account => children(account))
  }
}

module Dashboard = {
  @react.component
  let make = (~account, ~route: Routes.t, ~showBuyTez, ~setMainPage) => {
    let (accountsViewMode, setAccountsViewMode) = React.useState(_ => AccountsView.Mode.Simple)

    <>
      {switch route {
      | Accounts =>
        <AccountsView
          account
          mode=accountsViewMode
          setMode=setAccountsViewMode
          showBuyTez
          showOnboarding={() => setMainPage(_ => Homepage.AddAccountModal)}
        />
      | Nft => <NftView account />
      | Operations => <OperationsView account />
      | AddressBook => <AddressBookView />
      | Delegations => <DelegationsView />
      | Tokens => <TokensView />
      | Settings => <SettingsView />
      | Logs => <LogsView />
      | Batch => <GlobalBatchView />
      | Help => <HelpView />
      | NotFound =>
        <View> <Typography.Body1> {I18n.error404->React.string} </Typography.Body1> </View>
      }}
    </>
  }
}

let shouldDisplayNavbar = (
  ~accountsRequest: Umami.ApiRequest.t<Umami.PublicKeyHash.Map.map<Umami.Account.t>>,
  ~hasAccount,
  ~mainPageState: Homepage.state,
) =>
  switch accountsRequest {
  | Done(_) if !hasAccount => false
  | NotAsked => false
  | Loading(None) => false
  | Loading(Some(_)) => true
  | Done(_) =>
    switch mainPageState {
    | Onboarding
    | AddAccountModal => false
    | BuyTez(_) => false
    | Dashboard => true
    }
  }

module AppView = {
  @react.component
  let make = () => {
    let url = ReasonReactRouter.useUrl()
    let route = Routes.match_(url)

    let selectedAccount = StoreContext.SelectedAccount.useGetAtInit()
    let accountsRequest = StoreContext.Accounts.useRequest()
    let eulaSignature = StoreContext.useEulaSignature()
    let setEulaSignature = StoreContext.setEulaSignature()

    let hasAccount = selectedAccount != None

    let onSign = needSign => setEulaSignature(_ => needSign)

    let (mainPageState, setMainPage) = React.useState(_ => Homepage.Dashboard)

    React.useLayoutEffect1(() =>
      switch accountsRequest {
      | Done(_) if !hasAccount =>
        setMainPage(_ => Onboarding)
        None
      | _ =>
        setMainPage(_ => Dashboard)
        None
      }
    , [accountsRequest])

    let theme = ThemeContext.useTheme()

    let displayNavbar = shouldDisplayNavbar(~accountsRequest, ~hasAccount, ~mainPageState)

    let handleCloseBuyTezView = _ => setMainPage(_ => Dashboard)

    let toastBox = LogsContext.useToastBox()

    <>
      toastBox
      <DocumentContext>
        <View
          style={
            open Style
            array([styles["layout"], style(~backgroundColor=theme.colors.background, ())])
          }>
          <Header />
          {eulaSignature
            ? <DisclaimerModal onSign />
            : <View style={styles["main"]}>
                {switch selectedAccount {
                | Some(account) if displayNavbar => <NavBar account route />
                | Some(_)
                | None =>
                  <NavBar.Empty />
                }}
                <View style={styles["content"]}>
                  {switch mainPageState {
                  | Onboarding => <OnboardingView />
                  | AddAccountModal => <OnboardingView onClose={_ => setMainPage(_ => Dashboard)} />
                  | BuyTez(src) => <BuyTezView src onClose=handleCloseBuyTezView />
                  | Dashboard =>
                    <SelectedAccountView>
                      {account =>
                        <Dashboard
                          account
                          showBuyTez={url => setMainPage(_ => BuyTez(url))}
                          route
                          setMainPage
                        />}
                    </SelectedAccountView>
                  }}
                </View>
              </View>}
        </View>
      </DocumentContext>
    </>
  }
}

@react.component
let make = () =>
  <LogsContext>
    <ConfigFileContext>
      <ConfigContext>
        <ThemeContextWithConfig>
          <StoreContext>
            <GlobalBatchContext>
              <NoticesContext>
                <AppView />
                <SelectedAccountView>
                  {account => <BeaconConnectRequest account />}
                </SelectedAccountView>
              </NoticesContext>
            </GlobalBatchContext>
          </StoreContext>
        </ThemeContextWithConfig>
      </ConfigContext>
    </ConfigFileContext>
  </LogsContext>
