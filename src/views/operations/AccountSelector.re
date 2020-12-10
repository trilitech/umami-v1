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
  let make = (~account: Account.t) => {
    <View style=styles##inner>
      <Typography.Subtitle2>
        account.alias->React.string
      </Typography.Subtitle2>
      <Typography.Body1 colorStyle=`mediumEmphasis>
        account.address->React.string
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

let renderButton = (selectedAccount: option(Account.t)) =>
  <View style=styles##selectorContent>
    {selectedAccount->Belt.Option.mapWithDefault(<LoadingView />, account =>
       <AccountItem account />
     )}
  </View>;

let renderItem = (account: Account.t) => <AccountItem account />;

[@react.component]
let make = (~style=?) => {
  let account = StoreContext.SelectedAccount.useGet();
  let accounts = StoreContext.Accounts.useGetAll();

  let updateAccount = StoreContext.SelectedAccount.useSet();

  let items =
    accounts
    ->Belt.Map.String.valuesToArray
    ->Belt.SortArray.stableSortBy((a, b) =>
        Pervasives.compare(a.alias, b.alias)
      );

  <>
    <Typography.Overline1> I18n.t#account->React.string </Typography.Overline1>
    <View style=styles##spacer />
    <Selector
      items
      getItemValue={account => account.address}
      ?style
      onValueChange={value => updateAccount(value)}
      selectedValue=?{account->Belt.Option.map(account => account.address)}
      renderButton
      renderItem
    />
  </>;
};
