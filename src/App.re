open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "layout": style(~flex=1., ~flexDirection=`column, ()),
      "main": style(~flex=1., ~flexDirection=`row, ()),
      "content": style(~flex=1., ()),
    })
  );

module AppView = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    let route = Routes.match(url);

    let accounts = StoreContext.Accounts.useGetAll();
    let accountsRequest = StoreContext.Accounts.useRequest();

    let displayOnboarding =
      switch (accountsRequest) {
      | Done(_)
      | NotAsked when accounts->Map.String.size <= 0 => true
      | _ => false
      };

    let theme = ThemeContext.useTheme();

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
          {displayOnboarding ? React.null : <NavBar route />}
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
                 | Debug => <DebugView />
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

module ThemedView = {
  [@react.component]
  let make = () => {
    let confLoaded = ConfigContext.useLoaded();
    confLoaded
      ? {
        <ThemeContext> <AppView /> </ThemeContext>;
      }
      : <LoadingView />;
  };
};

[@react.component]
let make = () => {
  <LogsContext>
    <ConfigContext>
      <StoreContext> <ThemedView /> </StoreContext>
    </ConfigContext>
  </LogsContext>;
};
