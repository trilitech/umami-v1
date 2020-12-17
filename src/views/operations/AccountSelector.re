open ReactNative;

module AccountItem = {
  let styles =
    Style.(
      StyleSheet.create({
        "inner":
          style(
            ~height=44.->dp,
            ~marginHorizontal=20.->dp,
            ~justifyContent=`spaceBetween,
            (),
          ),
        "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
      })
    );

  [@react.component]
  let make =
      (~account: Account.t, ~token: option(Token.t)=?, ~showBalance=true) => {
    <View style=styles##inner>
      <View style=styles##info>
        <Typography.Subtitle2>
          account.alias->React.string
        </Typography.Subtitle2>
        {showBalance
           ? <AccountInfoBalance address={account.address} ?token />
           : React.null}
      </View>
      <Typography.Address fontSize=16.>
        account.address->React.string
      </Typography.Address>
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

let baseRenderButton =
    (~showBalance, ~token, selectedAccount: option(Account.t)) =>
  <View style=styles##selectorContent>
    {selectedAccount->Belt.Option.mapWithDefault(<LoadingView />, account =>
       <AccountItem account showBalance ?token />
     )}
  </View>;

let baseRenderItem = (~showBalance, ~token, account: Account.t) =>
  <AccountItem account showBalance ?token />;

///

let renderButton = baseRenderButton(~showBalance=true, ~token=None);

let renderItem = baseRenderItem(~showBalance=false, ~token=None);

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
    <Typography.Overline2> I18n.t#account->React.string </Typography.Overline2>
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
