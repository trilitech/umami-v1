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
let make = (~label, ~value: string, ~handleChange, ~error) => {
  let accounts = StoreContext.useAccounts();

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Option.getWithDefault(Belt.Map.String.empty)
    ->Belt.Map.String.valuesToArray
    ->Belt.SortArray.stableSortBy((a, b) =>
        Pervasives.compare(a.alias, b.alias)
      )
    ->Belt.Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  let (currentAccount, setCurrent) = React.useState(() => value);

  let balanceRequest = BalanceApiRequest.useLoad(currentAccount);

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <View
        style={Style.array([|AccountInfo.styles##balance, styles##balance|])}>
        {AccountInfo.balance(balanceRequest)}
      </View>
      <Selector
        items
        onValueChange={value => {
          setCurrent(_ => value);
          accounts
          ->Belt.Option.flatMap(accounts =>
              accounts->Belt.Map.String.get(value)
            )
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
