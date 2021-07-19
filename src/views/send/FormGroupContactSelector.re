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
  let make = (~account: Alias.t) => {
    <View style=styles##itemContainer>
      <Typography.Subtitle1> account.name->React.string </Typography.Subtitle1>
      <Typography.Address>
        (account.address :> string)->React.string
      </Typography.Address>
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

let renderItem = (account: Alias.t) => <Item account />;

let keyExtractor = (account: Alias.t) => (account.address :> string);

let renderLabel = (label, hasError) => {
  <FormLabel label hasError style=styles##label />;
};

let findAllAliases = (aliases, v) =>
  aliases->Array.keep((a: Alias.t) =>
    a.name
    ->Js.String2.trim
    ->Js.String2.toLowerCase
    ->Js.String2.startsWith(v->Js.String2.trim->Js.String2.toLowerCase)
  );

[@react.component]
let make =
    (
      ~label,
      ~filterOut: option(Alias.t),
      ~aliases: Belt.Map.String.t(Alias.t),
      ~value: FormUtils.Alias.any,
      ~handleChange: FormUtils.Alias.any => unit,
      ~error,
    ) => {
  let aliasArray =
    aliases
    ->Map.String.valuesToArray
    ->Array.keep((v: Alias.t) =>
        Some(v.address) != filterOut->Option.map(a => a.address)
      )
    ->SortArray.stableSortBy(Alias.compareName);

  let items =
    switch (value) {
    | AnyString("") => aliasArray->Array.slice(~offset=0, ~len=4)
    | Valid(Address(v)) => aliasArray->findAllAliases((v :> string))
    | AnyString(v) => aliasArray->findAllAliases(v)
    | Valid(Alias(_)) => [||]
    };

  let validAlias =
    switch (value) {
    | FormUtils.Alias.Valid(_) => true
    | FormUtils.Alias.AnyString(_) => false
    };

  let styleValidAlias =
    validAlias ? Style.(style(~fontWeight=`bold, ()))->Some : None;

  <FormGroup style=styles##formGroup>
    <Autocomplete
      keyPopover="formGroupContactSelector"
      value={
        switch (value) {
        | Valid(Alias(a)) => a.name
        | AnyString(s) => s
        | Valid(Address(s)) =>
          aliases
          ->Map.String.get((s :> string))
          ->Option.mapWithDefault((s :> string), a => a.name)
        }
      }
      handleChange={s =>
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
