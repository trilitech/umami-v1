open ReactNative;
open Belt;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=10, ()),
      "switchCmp": style(~height=10.->dp, ~width=20.->dp, ()),
      "switchThumb": style(~transform=[|scale(~scale=0.65)|], ()),
    })
  );

[@react.component]
let make = (~label, ~value, ~handleChange, ~error) => {
  let aliasesRequest = AliasApiRequest.useGetAliases();

  let hasError = error->Option.isSome;

  let items =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault([||])
    ->Array.map(((alias, address)) => {
        let account: Account.t = {alias, address};
        account;
      })
    ->Array.map(account =>
        {Selector.value: account.address, label: account.alias}
      );

  React.useEffect2(
    () => {
      if (value == "" && items->Array.size > 0) {
        let firstItemValue =
          items
          ->Array.get(0)
          ->Option.mapWithDefault("", firstItem => firstItem.value);

        if (firstItemValue != "") {
          handleChange(firstItemValue);
        };
      };
      None;
    },
    (value, items),
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
