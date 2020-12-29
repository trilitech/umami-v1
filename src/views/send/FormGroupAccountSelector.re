open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=11, ()),
      "label": style(~marginBottom=6.->dp, ()),
    })
  );

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

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Map.String.valuesToArray
    ->Belt.SortArray.stableSortBy((a, b) =>
        Pervasives.compare(a.alias, b.alias)
      );

  let (_currentAccount, setCurrent) = React.useState(() => value);

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        ?disabled
        getItemValue={account => account.address}
        onValueChange={value => {
          setCurrent(_ => value);
          accounts
          ->Belt.Map.String.get(value)
          ->Belt.Option.mapWithDefault("", a => a.address)
          ->handleChange;
        }}
        selectedValue=value
        renderButton={baseRenderButton(~token)}
        renderItem={baseRenderItem(~token)}
      />
    </View>
  </FormGroup>;
};
