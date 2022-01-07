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

module StateLenses = [%lenses type state = {read: bool}];
module DisclaimerForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "disclaimerText": style(~marginBottom=32.->dp, ~marginTop=24.->dp, ()),
      "checkboxLabel":
        style(
          ~fontSize=16.,
          ~alignItems=`flexStart,
          ~paddingBottom=20.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~onSign) => {
  let theme = ThemeContext.useTheme();

  let onAgree = () => {
    WertDisclaimer.sign();
  };

  let form: DisclaimerForm.api =
    DisclaimerForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        DisclaimerForm.Validation.(Schema(true_(Read)));
      },
      ~onSubmit=
        _ => {
          onAgree();
          onSign(false);
          None;
        },
      ~initialState={read: false},
      ~i18n=FormUtils.i18n,
      (),
    );

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <DocumentContext.ScrollView
      showsVerticalScrollIndicator=true style=styles##disclaimerText>
      <WertDisclaimerText />
    </DocumentContext.ScrollView>
    <View>
      <View>
        <CheckboxItem
          style=styles##checkboxLabel
          label=I18n.Disclaimer.agreement_checkbox
          labelFontWeightStyle=`regular
          labelStyle={Style.style(
            ~color=Typography.getColor(`highEmphasis, theme),
            (),
          )}
          value={form.values.read}
          handleChange={form.handleChange(Read)}
        />
      </View>
      <Buttons.SubmitPrimary
        text=I18n.Btn.disclaimer_agree
        onPress={_ => form.submit()}
        disabledLook={!formFieldsAreValids}
        style=FormStyles.formSubmit
      />
    </View>
  </>;
};
