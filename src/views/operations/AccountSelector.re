open ReactNative;

module AccountItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "inner":
          style(
            ~height=48.->dp,
            ~marginHorizontal=20.->dp,
            ~justifyContent=`spaceBetween,
            (),
          ),
        "text": style(~color="#FFF", ()),
      })
    );

  [@react.component]
  let make = (~alias, ~address) => {
    <View style=styles##inner>
      <Text style=styles##text> alias->React.string </Text>
      <Text style=styles##text> address->React.string </Text>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "button":
        style(
          ~height=68.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~borderColor="rgba(255,255,255,0.6)",
          ~borderWidth=1.,
          ~borderRadius=5.,
          (),
        ),
    })
  );

let renderButton = (selectedItem: option(Selector.item)) =>
  <View style=styles##button>
    {selectedItem->Belt.Option.mapWithDefault(<LoadingView />, item =>
       <AccountItem alias={item.label} address={item.value} />
     )}
  </View>;

let renderItem = (item: Selector.item) =>
  <AccountItem alias={item.label} address={item.value} />;

[@react.component]
let make = (~style=?) => {
  let account = StoreContext.useAccount();
  let accounts = StoreContext.useAccounts();

  let updateAccount = StoreContext.useUpdateAccount();

  let items =
    accounts
    ->Belt.Option.getWithDefault(Belt.Map.String.empty)
    ->Belt.Map.String.valuesToArray
    ->Belt.Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  <Selector
    items
    ?style
    onValueChange={value => updateAccount(value)}
    selectedValue=?{account->Belt.Option.map(account => account.address)}
    renderButton
    renderItem
  />;
};
