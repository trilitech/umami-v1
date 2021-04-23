open ReactNative;

let styles =
  Style.(StyleSheet.create({"label": style(~marginBottom=6.->dp, ())}));

let baseRenderButton = AccountSelector.baseRenderButton(~showAmount=Balance);

let baseRenderItem = AccountSelector.baseRenderItem(~showAmount=Nothing);

[@react.component]
let make =
    (
      ~label,
      ~value: string,
      ~handleChange,
      ~error,
      ~disabled=?,
      ~token: option(Token.t)=?,
    ) => {
  let accounts = StoreContext.Accounts.useGetAll();

  let hasError = error->Option.isSome;

  let items =
    accounts
    ->Map.String.valuesToArray
    ->SortArray.stableSortBy((a, b) => Pervasives.compare(a.alias, b.alias));

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        ?disabled
        getItemKey={account => account.address}
        onValueChange={account => {
          accounts->Map.String.get(account.address)->handleChange
        }}
        selectedValueKey=value
        renderButton={baseRenderButton(~token)}
        renderItem={baseRenderItem(~token)}
        keyPopover="formGroupAccountSelector"
      />
    </View>
  </FormGroup>;
};
