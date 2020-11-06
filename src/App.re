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

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();
  let route = Routes.match(url);

  <StoreContext>
    <ErrorsContext>
      <View style=styles##layout>
        <NavBar route />
        <View style=styles##main>
          <Header />
          <View style=styles##content>
            {switch (route) {
             | Accounts => <AccountsView />
             | Operations => <OperationsView />
             | AddressBook => <AddressBookView />
             | Debug => <DebugView />
             | NotFound =>
               <View>
                 <Typography.Body1>
                   "404 - Route Not Found :("->React.string
                 </Typography.Body1>
               </View>
             }}
          </View>
        </View>
      </View>
    </ErrorsContext>
  </StoreContext>;
};
