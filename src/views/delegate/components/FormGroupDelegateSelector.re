open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=11, ()),
      "label": style(~marginBottom=6.->dp, ()),
    })
  );

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error, ~disabled) => {
  let accounts = StoreContext.Accounts.useGetAllWithDelegates();

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Map.String.valuesToArray
    ->Belt.Array.keepMap(((account, delegate)) =>
        delegate->Belt.Option.isNone || disabled ? Some(account) : None
      )
    ->Belt.SortArray.stableSortBy((a, b) =>
        Js.String.localeCompare(a.alias, b.alias)->int_of_float
      );

  React.useEffect2(
    () => {
      if (value == "") {
        let firstItem = items->Belt.Array.get(0);
        firstItem->Common.Lib.Option.iter(account =>
          account.address->handleChange
        );
      };
      None;
    },
    (value, items),
  );

  <FormGroup style=styles##formGroup>
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
      />
    </View>
  </FormGroup>;
};
