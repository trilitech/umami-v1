open ReactNative;

module AccountItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~height=84.->dp, ~flexDirection=`row, ()),
        "inner": style(~justifyContent=`spaceBetween, ()),
        "border":
          style(
            ~backgroundColor=Colors.border,
            ~width=4.->dp,
            ~marginRight=11.->dp,
            ~borderTopRightRadius=4.,
            ~borderBottomRightRadius=4.,
            (),
          ),
        "title":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`bold, ()),
        "balance":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`bold, ()),
        "label":
          style(~color=Colors.lowText, ~fontSize=12., ~fontWeight=`_700, ()),
        "address":
          style(~color=Colors.stdText, ~fontSize=14., ~fontWeight=`_400, ()),
      })
    );

  [@react.component]
  let make = (~account) => {
    let balanceRequest = ApiRequest.useBalance(account);

    <View style=styles##container>
      <View style=styles##border />
      <View style=styles##inner>
        <Text style=styles##title> "Account 1"->React.string </Text>
        <Text style=styles##balance>
          {switch (balanceRequest) {
           | Done(Ok(balance)) => balance->React.string
           | Done(Error(error)) => error->React.string
           | NotAsked
           | Loading =>
             <ActivityIndicator
               animating=true
               size={ActivityIndicator_Size.exact(17.)}
               color=Colors.highIcon
             />
           }}
        </Text>
        <Text style=styles##label> "Address"->React.string </Text>
        <Text style=styles##address> account->React.string </Text>
      </View>
    </View>;
  };
};

module SendButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~position=`absolute,
            ~right=36.->dp,
            ~top=40.->dp,
            ~alignItems=`center,
            (),
          ),
        "iconContainer":
          style(
            ~width=40.->dp,
            ~height=40.->dp,
            ~justifyContent=`center,
            ~alignItems=`center,
            ~borderRadius=20.,
            ~backgroundColor=Colors.plainIconBack,
            ~marginBottom=6.->dp,
            (),
          ),
        "text":
          style(
            ~color=Colors.plainIconText,
            ~fontSize=12.,
            ~fontWeight=`_500,
            (),
          ),
      })
    );

  [@react.component]
  let make = () => {
    let (href, onPress) = Routes.useHrefAndOnPress(Routes.Send);

    <TouchableOpacity
      style=styles##button accessibilityRole=`link href onPress>
      <View style=styles##iconContainer>
        <Icon name=`send size=24. color=Colors.plainIconContent />
      </View>
      <Text style=styles##text> "SEND"->React.string </Text>
    </TouchableOpacity>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let account = StoreContext.useAccount();

  <> <Page> <AccountItem account /> </Page> <SendButton /> </>;
};
