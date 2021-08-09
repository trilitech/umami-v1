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

open UmamiCommon;
open ReactNative;

type inputType =
  | Selector
  | Text;

module BakerSelector = {
  module BakerItem = {
    let styles =
      Style.(
        StyleSheet.create({
          "inner":
            style(
              ~height=46.->dp,
              ~marginHorizontal=20.->dp,
              ~justifyContent=`spaceBetween,
              (),
            ),
        })
      );

    [@react.component]
    let make = (~baker: Delegate.t) => {
      <View style=styles##inner>
        <Typography.Subtitle2> baker.name->React.string </Typography.Subtitle2>
        <Typography.Address>
          (baker.address :> string)->React.string
        </Typography.Address>
      </View>;
    };
  };

  let styles =
    Style.(
      StyleSheet.create({
        "selectorContent":
          style(
            ~height=40.->dp,
            ~flexDirection=`row,
            ~alignItems=`center,
            ~flex=1.,
            (),
          ),
        "inner":
          style(~flex=1., ~flexDirection=`row, ~paddingLeft=20.->dp, ()),
        "address": style(~flexBasis=120.->dp, ~marginLeft=auto, ()),
      })
    );

  let renderButton = (selectedBaker: option(Delegate.t), hasError) =>
    <View style=styles##selectorContent>
      {selectedBaker->Option.mapWithDefault(<LoadingView />, baker =>
         <View style=styles##inner>
           <Typography.Subtitle2 colorStyle=?{hasError ? Some(`error) : None}>
             baker.name->React.string
           </Typography.Subtitle2>
           <Typography.Address
             colorStyle=?{hasError ? Some(`error) : None}
             numberOfLines=1
             style=styles##address>
             (baker.address :> string)->React.string
           </Typography.Address>
         </View>
       )}
    </View>;

  let renderItem = (baker: Delegate.t) => <BakerItem baker />;
};

module BakerInputTypeToogle = {
  let styles =
    Style.(
      StyleSheet.create({
        "inputTypeButton":
          style(~flexDirection=`row, ~alignItems=`center, ()),
        "inputTypeText": style(~lineHeight=16., ~marginLeft=7.->dp, ()),
      })
    );

  [@react.component]
  let make = (~inputType, ~onPress) => {
    let theme = ThemeContext.useTheme();
    <TouchableOpacity style=styles##inputTypeButton onPress>
      {switch (inputType) {
       | Selector =>
         <>
           <Icons.Edit size=14. color={theme.colors.iconPrimary} />
           <Typography.ButtonSecondary
             style=Style.(
               array([|
                 styles##inputTypeText,
                 style(~color=theme.colors.textPrimary, ()),
               |])
             )>
             I18n.btn#custom->React.string
           </Typography.ButtonSecondary>
         </>
       | Text =>
         <>
           <Icons.List size=14. color={theme.colors.iconPrimary} />
           <Typography.ButtonSecondary
             style=Style.(
               array([|
                 styles##inputTypeText,
                 style(~color=theme.colors.textPrimary, ()),
               |])
             )>
             I18n.btn#see_list->React.string
           </Typography.ButtonSecondary>
         </>
       }}
    </TouchableOpacity>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "formGroup": style(~marginBottom=0.->dp, ()),
      "labelContainer":
        style(
          ~marginBottom=6.->dp,
          ~flexDirection=`row,
          ~justifyContent=`spaceBetween,
          ~alignItems=`flexEnd,
          (),
        ),
    })
  );

[@react.component]
let make = (~label, ~value: option(string), ~handleChange, ~error) => {
  let bakersRequest = StoreContext.Bakers.useLoad();

  let hasError = error->Option.isSome;

  let items = bakersRequest->ApiRequest.getDoneOk->Option.getWithDefault([||]);

  let (inputType, setInputType) = React.useState(_ => Selector);

  React.useEffect4(
    () => {
      if (inputType == Selector) {
        if (value == None && items->Array.size > 0) {
          // if input selector and no value, select first entry
          let firstItem = items->Array.get(0);
          firstItem->Lib.Option.iter(baker =>
            (baker.address :> string)->Some->handleChange
          );
        } else if (!
                     items->Array.some(baker =>
                       Some((baker.address :> string)) == value
                     )) {
          // if input selector and value isn't in the item list : switch to input text
          setInputType(_ =>
            Text
          );
        };
      };

      None;
    },
    (value, items, inputType, setInputType),
  );

  let onPressInputType = _e => {
    setInputType(prevInputType => prevInputType == Selector ? Text : Selector);
    // reset value when switching inputType
    handleChange(None);
  };

  <FormGroup style=styles##formGroup>
    <View style=styles##labelContainer>
      <FormLabel label hasError />
      <BakerInputTypeToogle inputType onPress=onPressInputType />
    </View>
    <View>
      {switch (inputType) {
       | Selector =>
         <Selector
           items
           getItemKey={baker => (baker.address :> string)}
           onValueChange={b => (b.address :> string)->Some->handleChange}
           selectedValueKey=?value
           hasError
           renderButton=BakerSelector.renderButton
           renderItem=BakerSelector.renderItem
           keyPopover="formGroupBakerSelector"
         />
       | Text =>
         <ThemedTextInput
           value={value->Option.getWithDefault("")}
           onValueChange={v => handleChange(v == "" ? None : v->Some)}
           hasError
           placeholder={js|Enter baker's tz1 address|js}
           onClear={() => handleChange(None)}
         />
       }}
    </View>
    <FormError ?error />
  </FormGroup>;
};
