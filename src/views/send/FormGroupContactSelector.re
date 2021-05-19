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
      ~filterOut: option(Account.t),
      ~accounts,
      ~value: FormUtils.Account.any,
      ~handleChange: FormUtils.Account.any => unit,
      ~error,
    ) => {
  let accountsArray =
    accounts
    ->Map.String.valuesToArray
    ->Array.keep((v: Account.t) =>
        Some(v.address) != filterOut->Option.map(a => a.address)
      );

  let items =
    switch (value) {
    | AnyString("") => accountsArray->Array.slice(~offset=0, ~len=4)
    | Valid(Address(v))
    | AnyString(v) =>
      accountsArray->Array.keep(account =>
        account.alias
        ->Js.String2.trim
        ->Js.String2.toLowerCase
        ->Js.String2.startsWith(v->Js.String2.trim->Js.String2.toLowerCase)
      )
    | Valid(Account(_)) => [||]
    };

  let validAccount =
    switch (value) {
    | FormUtils.Account.Valid(_) => true
    | FormUtils.Account.AnyString(_) => false
    };

  let styleValidAccount =
    validAccount ? Style.(style(~fontWeight=`bold, ()))->Some : None;

  <FormGroup style=styles##formGroup>
    <Autocomplete
      keyPopover="formGroupContactSelector"
      value={
        switch (value) {
        | Valid(Address(s)) =>
          accounts->Map.String.get(s)->Option.mapWithDefault(s, a => a.alias)
        | Valid(Account(a)) => a.alias
        | AnyString(s) => s
        }
      }
      handleChange={s =>
        accountsArray
        ->Array.getBy(v => v.Account.alias == s)
        ->(
            fun
            | Some(v) => v->Account->FormUtils.Account.Valid
            | None when ReTaquito.Utils.validateAddress(s) == Valid =>
              s->FormUtils.Account.Address->FormUtils.Account.Valid
            | None => s->FormUtils.Account.AnyString
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
      style=Style.(arrayOption([|Some(styles##input), styleValidAccount|]))
    />
    <FormError ?error />
  </FormGroup>;
};
