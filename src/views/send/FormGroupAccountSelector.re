open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=11, ()),
      "switchCmp": style(~height=10.->dp, ~width=20.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
    })
  );

[@react.component]
let make = (~label, ~value, ~handleChange, ~error) => {
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
    <FormLabel label hasError />
    <Selector
      items
      onValueChange={value => handleChange(value)}
      selectedValue=value
      renderButton=AccountSelector.renderButton
      renderItem=AccountSelector.renderItem
    />
  </FormGroup>;
};
