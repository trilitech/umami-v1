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
      })
    );

  [@react.component]
  let make = (~alias, ~address) => {
    <View style=styles##inner>
      <Typography.Subtitle2> alias->React.string </Typography.Subtitle2>
      <Typography.Body1 colorStyle=`mediumEmphasis>
        address->React.string
      </Typography.Body1>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "selectorContent":
        style(
          ~height=68.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "spacer": style(~height=6.->dp, ()),
    })
  );

let renderButton = (selectedItem: option(Selector.item)) =>
  <View style=styles##selectorContent>
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
    ->Belt.Map.String.valuesToArray
    ->Belt.SortArray.stableSortBy((a, b) =>
        Pervasives.compare(a.alias, b.alias)
      )
    ->Belt.Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  <>
    <Typography.Overline1> "Account"->React.string </Typography.Overline1>
    <View style=styles##spacer />
    <Selector
      items
      ?style
      onValueChange={value => updateAccount(value)}
      selectedValue=?{account->Belt.Option.map(account => account.address)}
      renderButton
      renderItem
    />
  </>;
};
