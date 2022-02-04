/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;
let itemHeight = 54.;
let numItemsToDisplay = 4.;

module ContactKindSelector = {
  type kind = [ | `Default | ReCustomAuth.handledProvider];

  let styles =
    Style.(
      StyleSheet.create({
        "selector":
          style(
            ~minWidth=170.->dp,
            ~display=`flex,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~justifyContent=`flexEnd,
            (),
          ),
      })
    );

  let toStringKey =
    fun
    | `Default => "default"
    | #ReCustomAuth.handledProvider as p => ReCustomAuth.providerToString(p);

  let toStringRender =
    fun
    | `Default => I18n.Label.tezos_address
    | #ReCustomAuth.handledProvider as p =>
      ReCustomAuth.getProviderName(p) ++ " " ++ I18n.Label.provider_account;

  let renderItem = (provider: kind) =>
    <Typography.ButtonSecondary
      colorStyle=`highEmphasis fontSize=14. style=Menu.Item.styles##text>
      {provider->toStringRender->React.string}
    </Typography.ButtonSecondary>;

  let renderButton = (ft, _hasError) => {
    let render = (provider: kind) =>
      <Typography.ButtonSecondary
        colorStyle=`primary style=FormStyles.selector##item>
        {provider->toStringRender->Js.String2.toLocaleUpperCase->React.string}
      </Typography.ButtonSecondary>;

    <View>
      {switch (ft) {
       | Some(ft) => render(ft)
       | None => render(`Default)
       }}
    </View>;
  };

  [@react.component]
  let make = (~value, ~onValueChange) => {
    let theme = ThemeContext.useTheme();

    let chevron =
      <Icons.ChevronDown
        size=15.
        color={theme.colors.iconPrimary}
        style=Selector.styles##icon
      />;

    <Selector
      globalStyle=`none
      chevron
      items=[|`Default, `google|]
      getItemKey={kind => kind->toStringKey}
      innerStyle=styles##selector
      renderItem
      selectedValueKey={value->toStringKey}
      onValueChange
      renderButton
      keyPopover="ContactKindSelector"
    />;
  };
};

module Item = {
  let styles =
    Style.(
      StyleSheet.create({
        "innerItem":
          style(~flexDirection=`column, ~justifyContent=`spaceAround, ()),
        "accounticon": FormStyles.accountIcon,
        "itemContainer":
          style(
            ~height=itemHeight->dp,
            ~flexDirection=`row,
            ~paddingVertical=4.->dp,
            ~paddingHorizontal=20.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~account: Alias.t) => {
    <View style=styles##itemContainer>
      <AliasIcon style=styles##accounticon kind={account.kind} isHD=true />
      <View style=styles##innerItem>
        <Typography.Subtitle1>
          account.name->React.string
        </Typography.Subtitle1>
        <Typography.Address>
          (account.address :> string)->React.string
        </Typography.Address>
      </View>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~marginBottom=0.->dp, ()),
      "label":
        style(
          ~display=`flex,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginBottom=4.->dp,
          (),
        ),
      "customAuthAddress": style(~marginLeft=4.->dp, ()),
      "input": style(~paddingHorizontal=20.->dp, ()),
      "error": style(~marginBottom=5.->dp, ()),
      "inputNotice":
        style(
          ~height=20.->dp,
          ~justifyContent=`flexStart,
          ~marginTop=5.->dp,
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`baseline,
          (),
        ),
    })
  );

let renderItem = (account: Alias.t) => <Item account />;

let keyExtractor = (account: Alias.t) => (account.address :> string);

let findAllAliases = (aliases, v) =>
  aliases->Array.keep((a: Alias.t) =>
    a.name
    ->Js.String2.trim
    ->Js.String2.toLowerCase
    ->Js.String2.startsWith(v->Js.String2.trim->Js.String2.toLowerCase)
  );

open FormUtils.Alias;

let buildPlaceholder =
  fun
  | #ReCustomAuth.handledProvider as p =>
    I18n.Input_placeholder.provider_handler(
      p->ReCustomAuth.getProviderName,
      p->CustomAuthProviders.getHandleKindI18n,
    )
  | `Default => I18n.Input_placeholder.add_contact_or_tz;

let onBlurProvider = (valRef: React.ref(_), callback, provider, value) => {
  let f = s => {
    callback(Temp(s, Pending));
    ReCustomAuth.getPublicAddress(
      ~provider,
      s->ReCustomAuth.Handle.fromString,
    )
    ->Promise.get(
        fun
        | Ok(pkh) => {
            switch (valRef.current) {
            | AnyString("") => ()
            | _ =>
              let kind =
                Alias.Account(
                  CustomAuth({
                    provider,
                    handle: s->ReCustomAuth.Handle.fromString,
                  }),
                );

              Valid(Alias(Alias.make(~kind, ~name=s, pkh)))->callback;
            };
          }
        | Error(_e) => {
            let error =
              I18n.Form_input_error.invalid_handler(
                provider->ReCustomAuth.getProviderName,
                provider->CustomAuthProviders.getHandleKindI18n,
              );
            callback(Temp(s, Error(error)));
          },
      );
  };

  switch (value) {
  | FormUtils.Alias.AnyString(s) => f(s)
  | Temp(s, NotAsked) => f(s)
  | Temp(_, Error(_)) => ()
  | Temp(_, Pending) => ()
  | Valid(_) => ()
  };
};

[@react.component]
let make =
    (
      ~label,
      ~filterOut: option(Alias.t),
      ~aliases: PublicKeyHash.Map.map(Alias.t),
      ~value: FormUtils.Alias.any,
      ~handleChange: FormUtils.Alias.any => unit,
      ~error,
    ) => {
  let aliasArray =
    aliases
    ->PublicKeyHash.Map.valuesToArray
    ->Array.keep((v: Alias.t) =>
        Some(v.address) != filterOut->Option.map(a => a.address)
      )
    ->SortArray.stableSortBy(Alias.compareName);

  let theme = ThemeContext.useTheme();

  let (contactKind, setContactKind) =
    React.useState(() =>
      switch (value) {
      | Valid(Alias({kind: Some(Account(CustomAuth({provider})))})) => (
          provider :> ContactKindSelector.kind
        )
      | _ => `Default
      }
    );

  let items =
    switch (contactKind) {
    | #ReCustomAuth.handledProvider => [||]
    | `Default =>
      switch (value) {
      | AnyString("") => aliasArray->Array.slice(~offset=0, ~len=4)
      | Valid(Address(v)) => aliasArray->findAllAliases((v :> string))
      | AnyString(v) => aliasArray->findAllAliases(v)
      | Temp(_)
      | Valid(Alias(_)) => [||]
      }
    };

  let valueRef = React.useRef(value);
  valueRef.current = value;

  let validAlias =
    switch (value) {
    | FormUtils.Alias.Valid(_) => true
    | FormUtils.Alias.AnyString(_) => false
    | FormUtils.Alias.Temp(_) => false
    };

  let error =
    switch (contactKind, value) {
    | (`Default, _) => error
    | (#ReCustomAuth.handledProvider, Temp(_, Error(_))) => error
    | _ => None
    };

  let renderLabel = (label, hasError) => {
    <View style=styles##label>
      <FormLabel label hasError />
      <ContactKindSelector
        value=contactKind
        onValueChange={v => setContactKind(_ => v)}
      />
    </View>;
  };

  let customAuthAddress =
    switch (contactKind, value) {
    | (#ReCustomAuth.handledProvider, Valid(Alias(Alias.{address}))) =>
      <View style=styles##inputNotice>
        <Typography.Notice colorStyle=`positive fontSize=14.>
          I18n.Label.tz_address->React.string
        </Typography.Notice>
        <Typography.Address style=styles##customAuthAddress fontSize=12.>
          (address :> string)->React.string
        </Typography.Address>
      </View>

    | (`Default | #ReCustomAuth.handledProvider, Temp(_, Pending)) =>
      <ActivityIndicator
        style=styles##inputNotice
        animating=true
        size=ActivityIndicator_Size.small
        color={theme.colors.iconHighEmphasis}
      />

    | (`Default | #ReCustomAuth.handledProvider, _) =>
      <View style=styles##inputNotice />
    };

  let styleValidAlias =
    validAlias ? Style.(style(~fontWeight=`bold, ()))->Some : None;

  <FormGroup style=styles##formGroup>
    <Autocomplete
      keyPopover="formGroupContactSelector"
      value={
        switch (value) {
        | Valid(Alias(a)) => a.name
        | Temp(s, _) => s
        | AnyString(s) => s
        | Valid(Address(s)) =>
          aliases
          ->PublicKeyHash.Map.get(s)
          ->Option.mapWithDefault((s :> string), a => a.name)
        }
      }
      handleChange={s => {
        switch (contactKind) {
        | `Default =>
          aliasArray
          ->Array.getBy(v => v.Alias.name == s)
          ->(
              fun
              | Some(v) => v->Alias->FormUtils.Alias.Valid
              | None =>
                switch (s->PublicKeyHash.build) {
                | Ok(s) => s->FormUtils.Alias.Address->FormUtils.Alias.Valid
                | Error(_) => s->FormUtils.Alias.AnyString
                }
            )
          ->handleChange
        | #ReCustomAuth.handledProvider =>
          s == ""
            ? s->AnyString->handleChange
            : FormUtils.Alias.Temp(s, NotAsked)->handleChange
        }
      }}
      onBlur={_ => {
        switch (contactKind) {
        | #ReCustomAuth.handledProvider as p =>
          onBlurProvider(valueRef, handleChange, p, value)
        | `Default => ()
        }
      }}
      error
      list=items
      clearButton=true
      renderItem
      keyExtractor
      placeholder={buildPlaceholder(contactKind)}
      renderLabel={renderLabel(label)}
      itemHeight
      numItemsToDisplay
      style=Style.(arrayOption([|Some(styles##input), styleValidAlias|]))
    />
    {error == None
       ? customAuthAddress : <FormError style=styles##error ?error />}
  </FormGroup>;
};
