open ReactNative;

let itemHeight = 54.;
let numItemsToDisplay = 4.;

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "itemContainer":
          style(
            ~height=itemHeight->dp,
            ~flexDirection=`column,
            ~justifyContent=`spaceAround,
            ~paddingVertical=4.->dp,
            ~paddingHorizontal=20.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~account: Account.t) => {
    <View style=styles##itemContainer>
      <Typography.Subtitle1>
        account.alias->React.string
      </Typography.Subtitle1>
      <Typography.Address fontSize=16.>
        account.address->React.string
      </Typography.Address>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~zIndex=10, ()),
      "label": style(~marginBottom=6.->dp, ()),
      "input": style(~paddingHorizontal=20.->dp, ()),
    })
  );

let renderItem = (account: Account.t) => <Item account />;

let keyExtractor = (account: Account.t) => account.address;

let renderLabel = (label, hasError) => {
  <FormLabel label hasError style=styles##label />;
};

[@react.component]
let make = (~label, ~value: string, ~handleChange, ~error) => {
  let aliasesRequest = StoreContext.Aliases.useRequest();

  let accounts =
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.mapWithDefault([||], Map.String.valuesToArray);

  let items =
    accounts->Array.keep(account =>
      account.alias
      ->Js.String2.trim
      ->Js.String2.toLowerCase
      ->Js.String2.startsWith(value->Js.String2.trim->Js.String2.toLowerCase)
    );

  <FormGroup style=styles##formGroup>
    <Autocomplete
      value
      handleChange
      error
      list=items
      renderItem
      keyExtractor
      renderLabel={renderLabel(label)}
      itemHeight
      numItemsToDisplay
      style=styles##input
    />
  </FormGroup>;
};