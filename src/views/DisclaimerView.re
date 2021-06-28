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
      "title": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "updateNotice": style(~textAlign=`center, ()),
      "subtitle": style(~textAlign=`center, ()),
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
  let onAgree = () => {
    Disclaimer.sign();
  };

  let form: DisclaimerForm.api =
    DisclaimerForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        DisclaimerForm.Validation.(Schema(true_(Read)));
      },
      ~onSubmit=
        _ => {
          onAgree()->ignore;
          onSign(false);
          None;
        },
      ~initialState={read: false},
      ~i18n=FormUtils.i18n,
      (),
    );

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <ModalFormView>
    <Typography.Headline style=styles##title>
      I18n.title#disclaimer->React.string
    </Typography.Headline>
    <Typography.Body2 colorStyle=`mediumEmphasis style=styles##updateNotice>
      {I18n.disclaimer#last_updated(Disclaimer.updateTime)->React.string}
    </Typography.Body2>
    <Typography.Overline1 colorStyle=`highEmphasis style=styles##subtitle>
      I18n.disclaimer#please_sign->React.string
    </Typography.Overline1>
    <DocumentContext.ScrollView
      showsVerticalScrollIndicator=true style=styles##disclaimerText>
      <DisclaimerText />
    </DocumentContext.ScrollView>
    <View>
      <View>
        <CheckboxItem
          style=styles##checkboxLabel
          label=I18n.disclaimer#agreement_checkbox
          labelFontWeightStyle=`regular
          value={form.values.read}
          handleChange={form.handleChange(Read)}
        />
      </View>
      <Buttons.SubmitPrimary
        text=I18n.btn#disclaimer_agree
        onPress={_ => form.submit()}
        disabledLook={!formFieldsAreValids}
        style=FormStyles.formSubmit
      />
    </View>
  </ModalFormView>; /* style=Style.(array([|style(~align=`center, ())|])) */
};
