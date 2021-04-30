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
    (
      ~label,
      ~filterOut,
      ~accounts,
      ~value: FormUtils.Account.t,
      ~handleChange,
      ~error,
    ) => {
  let accountsArray =
    accounts
    ->Map.String.valuesToArray
    ->Array.keep((v: Account.t) => v.address != filterOut);

  let items =
    switch (value) {
    | Address("") => accountsArray->Array.slice(~offset=0, ~len=4)
    | Address(v) =>
      accountsArray->Array.keep(account =>
        account.alias
        ->Js.String2.trim
        ->Js.String2.toLowerCase
        ->Js.String2.startsWith(v->Js.String2.trim->Js.String2.toLowerCase)
      )
    | Account(_) => [||]
    };

  let validAlias =
    accounts->Map.String.some((_, v) =>
      v.alias == value->FormUtils.Account.alias
      || v.address == value->FormUtils.Account.address
    );
  let styleValidAlias =
    validAlias ? Style.(style(~fontWeight=`bold, ()))->Some : None;

  <FormGroup style=styles##formGroup>
    <Autocomplete
      keyPopover="formGroupContactSelector"
      value={
        switch (value) {
        | Address(s) =>
          accounts->Map.String.get(s)->Option.mapWithDefault(s, a => a.alias)
        | Account(a) => a.alias
        }
      }
      handleChange={s =>
        accountsArray
        ->Array.getBy(v => v.Account.alias == s)
        ->Option.mapWithDefault(FormUtils.Account.Address(s), account =>
            account->FormUtils.Account.Account
          )
        ->handleChange
      }
      error
      list=items
      clearButton=true
      renderItem
      keyExtractor
      placeholder=I18n.input_placeholder#add_contact_or_tz
      renderLabel={renderLabel(label)}
      itemHeight
      numItemsToDisplay
      style=Style.(arrayOption([|Some(styles##input), styleValidAlias|]))
    />
    <FormError ?error />
  </FormGroup>;
};
