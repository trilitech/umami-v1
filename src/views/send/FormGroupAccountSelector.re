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

  let (_currentAccount, setCurrent) = React.useState(() => value);

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        ?disabled
        getItemValue={account => account.address}
        onValueChange={value => {
          setCurrent(_ => value);
          accounts
          ->Map.String.get(value)
          ->Option.mapWithDefault("", a => a.address)
          ->handleChange;
        }}
        selectedValue=value
        renderButton={baseRenderButton(~token)}
        renderItem={baseRenderItem(~token)}
        keyPopover="formGroupAccountSelector"
      />
    </View>
  </FormGroup>;
};
