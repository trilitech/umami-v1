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

let styles =
  Style.(
    StyleSheet.create({
      "formRowInputs":
        style(~flexDirection=`row, ~justifyContent=`center, ()),
      "formRowInputsSeparator": style(~width=13.->dp, ()),
      "formRowInput":
        style(
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=0.->dp,
          ~marginVertical=5.->dp,
          (),
        ),
    })
  );

let xtzDecoration = (~style) =>
  <Typography.Body1 style> I18n.t#xtz->React.string </Typography.Body1>;

[@react.component]
let make = (~operation, ~token, ~form: SendForm.api) => {
  let (operationSimulateRequest, sendOperationSimulate) =
    StoreContext.Operations.useSimulate();

  React.useEffect0(() => {
    if (form.values.recipient != AnyString("") && form.values.amount != "") {
      sendOperationSimulate(operation)
      ->Future.tapOk(dryRun => {
          form.handleChange(Fee, dryRun.fee->Tez.toString);
          form.handleChange(GasLimit, dryRun.gasLimit->string_of_int);
          form.handleChange(StorageLimit, dryRun.storageLimit->string_of_int);
          form.setFieldValue(DryRun, Some(dryRun));
        })
      ->ignore;
    };

    None;
  });

  let theme = ThemeContext.useTheme();

  <View>
    <View style=styles##formRowInputs>
      <FormGroupXTZInput
        label=I18n.label#fee
        value={form.values.fee}
        handleChange={fee => form.handleChange(Fee, fee)}
        error={form.getFieldError(Field(Fee))}
        style=styles##formRowInput
        decoration=FormGroupXTZInput.xtzDecoration
      />
      {{
         <>
           <View style=styles##formRowInputsSeparator />
           <FormGroupTextInput
             label=I18n.label#gas_limit
             value={form.values.gasLimit}
             handleChange={form.handleChange(GasLimit)}
             error={form.getFieldError(Field(GasLimit))}
             style=styles##formRowInput
           />
         </>;
       }
       ->ReactUtils.onlyWhen(token != None)}
      {{
         <>
           <View style=styles##formRowInputsSeparator />
           <FormGroupTextInput
             label=I18n.label#storage_limit
             value={form.values.storageLimit}
             handleChange={form.handleChange(StorageLimit)}
             error={form.getFieldError(Field(StorageLimit))}
             style=styles##formRowInput
           />
         </>;
       }
       ->ReactUtils.onlyWhen(token != None)}
    </View>
    <FormGroupCheckbox
      label=I18n.label#force_low_fee
      value={form.values.forceLowFee}
      handleChange={form.handleChange(ForceLowFee)}
      error={form.getFieldError(Field(ForceLowFee))}
    />
    {ReactUtils.onlyWhen(
       <View
         style=Style.(
           array([|
             StyleSheet.absoluteFillObject,
             style(
               ~backgroundColor=theme.colors.background,
               ~opacity=0.87,
               (),
             ),
           |])
         )>
         <LoadingView />
       </View>,
       operationSimulateRequest->ApiRequest.isLoading,
     )}
  </View>;
};
