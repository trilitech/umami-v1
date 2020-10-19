open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "layout":
        style(~flex=1., ~flexDirection=`row, ~backgroundColor="#000000", ()),
      "main": style(~flex=1., ()),
      "content": style(~flex=1., ()),
    })
  );

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();
  let route = Routes.match(url);

  <StoreContext>
    <View style=styles##layout>
      <NavBar route />
      <View style=styles##main>
        <Header />
        <View style=styles##content>
          {switch (route) {
           | Home => <HomeView />
           | Send => <SendView />
           | Operations => <OperationsView />
           | Debug => <DebugView />
           | NotFound =>
             <View>
               <Text> "404 - Route Not Found :("->React.string </Text>
             </View>
           }}
        </View>
      </View>
    </View>
  </StoreContext>;
};
