open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=11, ()),
      "label": style(~marginBottom=6.->dp, ()),
    })
  );

[@react.component]
let make = (~label, ~value: option(Account.t), ~handleChange, ~error) => {
  let accounts = StoreContext.useAccounts();

  let hasError = error->Belt.Option.isSome;

  let items =
    accounts
    ->Belt.Option.getWithDefault(Belt.Map.String.empty)
    ->Belt.Map.String.valuesToArray
    ->Belt.Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <Selector
      items
      onValueChange={value =>
        accounts
        ->Belt.Option.flatMap(accounts =>
            accounts->Belt.Map.String.get(value)
          )
        ->handleChange
      }
      selectedValue={
        value->Belt.Option.mapWithDefault("", v => v.Account.address)
      }
      renderButton=AccountSelector.renderButton
      renderItem=AccountSelector.renderItem
    />
  </FormGroup>;
};
