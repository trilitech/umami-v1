open ReactNative;
open UmamiCommon;

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
      <Typography.Address> account.address->React.string </Typography.Address>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~marginBottom=0.->dp, ()),
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
let make =
    (~label, ~filterOut, ~accounts, ~value: string, ~handleChange, ~error) => {
  let accountsArray =
    accounts
    ->Map.String.valuesToArray
    ->Array.keep((v: Account.t) => v.address != filterOut);

  React.useEffect1(
    () => {
      accounts
      ->Map.String.get(value)
      ->Lib.Option.iter((v: Account.t) => handleChange(v.alias));
      None;
    },
    [|value|],
  );

  let items =
    value == ""
      ? accountsArray->Array.slice(~offset=0, ~len=4)
      : accountsArray->Array.keep(account =>
          account.alias
          ->Js.String2.trim
          ->Js.String2.toLowerCase
          ->Js.String2.startsWith(
              value->Js.String2.trim->Js.String2.toLowerCase,
            )
        );

  <FormGroup style=styles##formGroup>
    <Autocomplete
      keyPopover="formGroupContactSelector"
      value
      handleChange
      error
      list=items
      clearButton=true
      renderItem
      keyExtractor
      placeholder=I18n.input_placeholder#add_contact_or_tz
      renderLabel={renderLabel(label)}
      itemHeight
      numItemsToDisplay
      style=styles##input
    />
    <FormError ?error />
  </FormGroup>;
};
