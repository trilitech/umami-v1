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
    | BuyTez
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
  };

  [@react.component]
  let make = (~src, ~onClose) => {
    Js.log(System.isDev);
    <Page>
      <Page.Header
        left={
          <Typography.Body1
            colorStyle=`highEmphasis
            style=Style.(style(~marginTop=12.->dp, ()))>
            I18n.expl#external_service->React.string
          </Typography.Body1>
        }
        right={<CloseButton onClose />}>
        ReasonReact.null
      </Page.Header>
      {IFrame.createElement({
         src,
         width: "100%",
         height: "100%",
         allow: "camera *; microphone *",
         frameBorder: "0",
       })}
    </Page>;
  };
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
    let (accountsViewMode, setAccountsViewMode) =
      React.useState(_ => AccountsView.Mode.Simple);

    let (onboardingState, setOnboardingState) =
      React.useState(_ =>
        switch (accountsRequest) {
        | Done(_) when accounts->PublicKeyHash.Map.size <= 0 => Homepage.Onboarding
        | NotAsked
        | Loading(None)
        | Loading(Some(_))
        | Done(_) => Dashboard
        }
      );

    React.useLayoutEffect1(
      () =>
        switch (accountsRequest) {
        | Done(_) when accounts->PublicKeyHash.Map.size <= 0 =>
          setOnboardingState(_ => Onboarding);
          None;
        | _ =>
          setOnboardingState(_ => Dashboard);
          None;
        },
      [|accountsRequest|],
    );

    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    React.useLayoutEffect1(
      () =>
        switch (onboardingState) {
        | BuyTez =>
          openAction();
          None;
        | _ => None
        },
      [|onboardingState|],
    );

    let theme = ThemeContext.useTheme();

    let (wertURL, setWertURL) = React.useState(() => None);

    let buyTez = (address: PublicKeyHash.t) => {
      closeAction();
      let widget =
        ReWert.Widget.make({
          container_id: "wert-widget",
          partner_id:
            System.isDev
              ? "01F8DFQRA460MG8EMEP6E0RQQT" : "01FN6APWJ68N2PWC22YDJW4D5W",
          origin:
            System.isDev
              ? "https://sandbox.wert.io" : "https://widget.wert.io",
          commodity: "XTZ",
          commodities: "XTZ",
          address: (address :> string),
          theme: theme.dark ? "dark" : "light",
        });
      setWertURL(_ => Some(widget->ReWert.Widget.getEmbedUrl));
    };

    let displayNavbar = {
      switch (accountsRequest) {
      | Done(_) when accounts->PublicKeyHash.Map.size <= 0 => false
      | NotAsked => false
      | Loading(None) => false
      | Loading(Some(_)) => true
      | Done(_) =>
        switch (onboardingState) {
        | Onboarding
        | AddAccountModal => false
        | BuyTez => wertURL == None
        | Dashboard => true
        }
      };
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
                 {switch (onboardingState) {
                  | Onboarding => <OnboardingView />
                  | AddAccountModal =>
                    <OnboardingView
                      onClose={_ => setOnboardingState(_ => Dashboard)}
                    />
                  | BuyTez
                  | Dashboard =>
                    <>
                      {wertURL->Option.mapWithDefault(
                         switch (route) {
                         | Accounts =>
                           <AccountsView
                             mode=accountsViewMode
                             setMode=setAccountsViewMode
                             showOnboarding={() =>
                               setOnboardingState(_ =>
                                 accountsViewMode == Simple
                                   ? BuyTez : AddAccountModal
                               )
                             }
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
                             <Typography.Body1>
                               I18n.t#error404->React.string
                             </Typography.Body1>
                           </View>
                         },
                         src =>
                         <BuyTezView
                           src
                           onClose={_ => {
                             setWertURL(_ => None);
                             setOnboardingState(_ => Dashboard);
                           }}
                         />
                       )}
                    </>
                  }}
               </View>
               <ModalAction visible=visibleModal onRequestClose=closeAction>
                 <WertView
                   submit=buyTez
                   closeAction={_ => {
                     closeAction();
                     if (wertURL == None) {
                       setOnboardingState(_ => Dashboard);
                     };
                   }}
                 />
               </ModalAction>
             </View>}
      </View>
    </DocumentContext>;
  };
};

[@react.component]
let make = () => {
  <LogsContext>
    <ConfigContext>
      <ThemeContext>
        <StoreContext> <AppView /> <BeaconConnectRequest /> </StoreContext>
      </ThemeContext>
    </ConfigContext>
  </LogsContext>;
};
