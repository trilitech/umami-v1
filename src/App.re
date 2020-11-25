open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "layout":
        style(
          ~flex=1.,
          ~flexDirection=`row,
          ~backgroundColor=Colors.contentBackground,
          (),
        ),
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

    let bodyView = () => {
      <View style=styles##content>
        {switch (route) {
         | Accounts => <AccountsView />
         | Operations => <OperationsView />
         | AddressBook => <AddressBookView />
         | Delegations => <DelegationsView />
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
      <View style=styles##layout>
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
  <ErrorsContext>
    <ConfigContext> <StoreContext> <AppView /> </StoreContext> </ConfigContext>
  </ErrorsContext>;
};
