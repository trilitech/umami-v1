open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "layout": style(~flex=1., ~flexDirection=`row, ()),
      "main": style(~flex=1., ()),
      "content": style(~flex=1., ()),
    })
  );

module AppView = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();
    let route = Routes.match(url);

    let confLoaded = ConfigContext.useLoaded();

    let theme = ThemeContext.useTheme();

    let bodyView = () => {
      <View style=styles##content>
        {switch (route) {
         | Accounts => <AccountsView />
         | Operations => <OperationsView />
         | AddressBook => <AddressBookView />
         | Delegations => <DelegationsView />
         | Tokens => <TokensView />
         | Debug => <DebugView />
         | NotFound =>
           <View>
             <Typography.Body1>
               I18n.t#error404->React.string
             </Typography.Body1>
           </View>
         }}
      </View>;
    };

    <DocumentContext>
      <View
        style=Style.(
          array([|
            styles##layout,
            style(~backgroundColor=theme.colors.background, ()),
          |])
        )>
        <NavBar route />
        <View style=styles##main>
          <Header />
          {confLoaded ? bodyView() : <LoadingView />}
        </View>
      </View>
    </DocumentContext>;
  };
};

[@react.component]
let make = () => {
  <LogsContext>
    <ConfigContext>
      <StoreContext> <ThemeContext> <AppView /> </ThemeContext> </StoreContext>
    </ConfigContext>
  </LogsContext>;
};
