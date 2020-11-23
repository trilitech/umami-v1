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
  let accounts = StoreContext.useAccountsWithDelegates();

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Map.String.valuesToArray
    ->Belt.Array.keepMap(((account, delegate)) =>
        delegate->Belt.Option.isNone
          ? Some({Selector.value: account.address, label: account.alias})
          : None
      )
    ->Belt.SortArray.stableSortBy((a, b) =>
        Js.String.localeCompare(a.label, b.label)->int_of_float
      );

  let balanceRequest = BalanceApiRequest.useLoad(value);

  React.useEffect2(
    () => {
      if (value == "") {
        let firstItem = items->Belt.Array.get(0);
        firstItem->Common.Lib.Option.iter(item => item.value->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <View
        style={Style.array([|AccountInfo.styles##balance, styles##balance|])}>
        {AccountInfo.balance(balanceRequest)}
      </View>
      <Selector
        items
        onValueChange=handleChange
        selectedValue=value
        renderButton=AccountSelector.renderButton
        renderItem=AccountSelector.renderItem
      />
    </View>
  </FormGroup>;
};
