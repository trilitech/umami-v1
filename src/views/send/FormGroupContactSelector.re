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
let make = (~label, ~value: string, ~handleChange, ~error) => {
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
      if (value == "" && items->Array.size > 0) {
        switch (items->Array.get(0)) {
        | Some(i) => i.value->handleChange
        | None => ()
        };
      };
      None;
    },
    (value, accounts),
  );

  let onValueChange = value => value->handleChange;

  <FormGroup style=styles##formGroup>
    <FormLabel label hasError style=styles##label />
    <Selector
      items
      onValueChange
      selectedValue=value
      renderButton=AccountSelector.renderButton
      renderItem=AccountSelector.renderItem
    />
  </FormGroup>;
};
