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

module SecretItem = {
  let itemStyles =
    Style.(
      StyleSheet.create({
        "inner": style(~height=44.->dp, ~justifyContent=`spaceBetween, ()),
        "info": style(~flexDirection=`row, ~justifyContent=`spaceBetween, ()),
      })
    );

  [@react.component]
  let make = (~style as paramStyle=?, ~secret: Secret.derived) => {
    <View style=Style.(arrayOption([|Some(itemStyles##inner), paramStyle|]))>
      <View style=itemStyles##info>
        <Typography.Subtitle2>
          secret.secret.name->React.string
        </Typography.Subtitle2>
      </View>
      <Typography.Address>
        {secret.secret.derivationPath
         ->DerivationPath.Pattern.toString
         ->React.string}
      </Typography.Address>
    </View>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "label": style(~marginBottom=6.->dp, ()),
      "itemInSelector": style(~marginHorizontal=20.->dp, ()),
      "selectorContent":
        style(
          ~height=66.->dp,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~flex=1.,
          (),
        ),
      "spacer": style(~height=6.->dp, ()),
    })
  );

let renderButton = (selectedSecret: option(Secret.derived), _hasError) =>
  <View style=styles##selectorContent>
    {selectedSecret->Option.mapWithDefault(<LoadingView />, secret =>
       <SecretItem style=styles##itemInSelector secret />
     )}
  </View>;

let renderItem = (secret: Secret.derived) =>
  <SecretItem style=styles##itemInSelector secret />;

[@react.component]
let make = (~label, ~value: option(string), ~handleChange, ~error, ~disabled) => {
  let secretsRequest = StoreContext.Secrets.useLoad();

  let hasError = error->Option.isSome;

  let items =
    secretsRequest
    ->ApiRequest.getWithDefault([||])
    ->SortArray.stableSortBy((a, b) =>
        Js.String.localeCompare(b.secret.name, a.secret.name)->int_of_float
      );

  React.useEffect2(
    () => {
      if (value == None) {
        let firstItem = items->Array.get(0);
        firstItem->Option.iter(secret => secret->handleChange);
      };
      None;
    },
    (value, items),
  );

  <FormGroup>
    <FormLabel label hasError style=styles##label />
    <View>
      <Selector
        items
        getItemKey={secret => secret.index->string_of_int}
        onValueChange=handleChange
        selectedValueKey=?value
        renderButton
        renderItem
        disabled
        keyPopover="formGroupSecretSelector"
      />
    </View>
  </FormGroup>;
};
