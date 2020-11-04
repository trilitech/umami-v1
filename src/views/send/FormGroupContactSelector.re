open ReactNative;
open Belt;

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=10, ()),
      "label": style(~marginBottom=6.->dp, ()),
    })
  );

[@react.component]
let make = (~label, ~value: option(Account.t), ~handleChange, ~error) => {
  let aliasesRequest = AliasApiRequest.useGetAliases();

  let hasError = error->Option.isSome;

  let accounts =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault([||])
    ->Array.map(((alias, address)) => {Account.{alias, address}});

  let items =
    accounts->Array.map(account =>
      {Selector.value: account.address, label: account.alias}
    );

  React.useEffect2(
    () => {
      if (value == None && items->Array.size > 0) {
        let firstItem = accounts->Array.get(0);

        if (firstItem != None) {
          handleChange(firstItem);
        };
      };
      None;
    },
    (value, items),
  );

  let onValueChange = value => {
    Array.getBy(accounts, acc => acc.address == value)->handleChange;
  };

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <Selector
      items
      onValueChange
      selectedValue=?{value->Belt.Option.map(account => account.address)}
      renderButton=AccountSelector.renderButton
      renderItem=AccountSelector.renderItem
    />
  </FormGroup>;
};
