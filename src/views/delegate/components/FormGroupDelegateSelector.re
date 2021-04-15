open ReactNative;
open UmamiCommon;

let styles =
  Style.(StyleSheet.create({"label": style(~marginBottom=6.->dp, ())}));

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error, ~disabled) => {
  let accounts = StoreContext.Accounts.useGetAllWithDelegates();

  let hasError = error->Option.isSome;

  let items =
    accounts
    ->Map.String.valuesToArray
    ->Array.keepMap(((account, delegate)) =>
        delegate->Option.isNone || disabled ? Some(account) : None
      )
    ->SortArray.stableSortBy((a, b) => Pervasives.compare(a.alias, b.alias));

  React.useEffect2(
    () => {
      if (value == "") {
        let firstItem = items->Array.get(0);
        firstItem->Lib.Option.iter(account => account.address->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        getItemValue={account => account.address}
        onValueChange=handleChange
        selectedValue=value
        renderButton=AccountSelector.renderButton
        renderItem=AccountSelector.renderItem
        disabled
        keyPopover="formGroupDelegateSelector"
      />
    </View>
  </FormGroup>;
};
