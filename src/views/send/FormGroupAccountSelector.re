open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=11, ()),
      "label": style(~marginBottom=6.->dp, ()),
      "balance": style(~position=`absolute, ~right=80.->dp, ~top=12.->dp, ()),
    })
  );

[@react.component]
let make =
    (
      ~label,
      ~value: string,
      ~handleChange,
      ~error,
      ~token: option(Token.t)=?,
    ) => {
  let accounts = StoreContext.Accounts.useGetAll();

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Map.String.valuesToArray
    ->Belt.SortArray.stableSortBy((a, b) =>
        Pervasives.compare(a.alias, b.alias)
      )
    ->Belt.Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  let (currentAccount, setCurrent) = React.useState(() => value);

  let balanceRequest = StoreContext.Balance.useLoad(currentAccount);
  let balanceTokenRequest =
    StoreContext.BalanceToken.useLoad(
      currentAccount,
      token->Belt.Option.map(token => token.address),
    );

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <View style=styles##balance>
        <AccountInfoBalance
          ?token
          balanceRequest={
            token->Belt.Option.isSome ? balanceTokenRequest : balanceRequest
          }
        />
      </View>
      <Selector
        items
        onValueChange={value => {
          setCurrent(_ => value);
          accounts
          ->Belt.Map.String.get(value)
          ->Belt.Option.mapWithDefault("", a => a.address)
          ->handleChange;
        }}
        selectedValue=value
        renderButton=AccountSelector.renderButton
        renderItem=AccountSelector.renderItem
      />
    </View>
  </FormGroup>;
};
