open ReactNative;

type amountDisplay =
  | Balance
  | Nothing
  | Amount(React.element);

let itemStyles =
  Style.(
    StyleSheet.create({
      "itemInSelector": style(~marginHorizontal=20.->dp, ()),
      "inner": style(~height=44.->dp, ~justifyContent=`spaceBetween, ()),
      "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
    })
  );

module AccountItem = {
  [@react.component]
  let make =
      (
        ~style as paramStyle=?,
        ~account: Account.t,
        ~token: option(Token.t)=?,
        ~showAmount=Balance,
      ) => {
    <View style=Style.(arrayOption([|Some(itemStyles##inner), paramStyle|]))>
      <View style=itemStyles##info>
        <Typography.Subtitle2>
          account.alias->React.string
        </Typography.Subtitle2>
        {switch (showAmount) {
         | Balance => <AccountInfoBalance address={account.address} ?token />
         | Nothing => React.null
         | Amount(e) => e
         }}
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
    (~showAmount, ~token, selectedAccount: option(Account.t)) =>
  <View style=styles##selectorContent>
    {selectedAccount->Belt.Option.mapWithDefault(<LoadingView />, account =>
       <AccountItem
         style=itemStyles##itemInSelector
         account
         showAmount
         ?token
       />
     )}
  </View>;

let baseRenderItem = (~showAmount, ~token, account: Account.t) =>
  <AccountItem style=itemStyles##itemInSelector account showAmount ?token />;

let renderButton = baseRenderButton(~showAmount=Balance, ~token=None);

let renderItem = baseRenderItem(~showAmount=Nothing, ~token=None);

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
