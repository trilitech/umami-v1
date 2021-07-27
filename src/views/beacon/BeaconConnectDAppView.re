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

module Form = {
  module StateLenses = [%lenses type state = {pairingRequest: string}];

  include ReForm.Make(StateLenses);
};

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "verticalFormAction":
        StyleSheet.flatten([|
          FormStyles.verticalFormAction,
          style(~marginTop=12.->dp, ()),
        |]),
    })
  );

[@react.component]
let make = (~closeAction) => {
  let form =
    Form.use(
      ~schema={
        Form.Validation.(Schema(nonEmpty(PairingRequest)));
      },
      ~onSubmit=
        ({state}) => {
          Js.log(state.values.pairingRequest);

          None;
        },
      ~initialState={pairingRequest: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <View>
      <View style=FormStyles.header>
        <Typography.Headline style=styles##title>
          "Connect to DApp with pairing request"->React.string
        </Typography.Headline>
      </View>
      <FormGroupTextInput
        label="DApp pairing request"
        value={form.values.pairingRequest}
        placeholder={j|e.g. BSdNU2tFbvtHvFpWR7rjrHyna1VQkAFnz4CmDTqkohdCx4FS51WUpc5Z9YoNJqbtZpoDNJfencTaDp23fWQqcyL54F75puvwCfmC1RCn11RLyFHrCYKo7uJ7a9KR8txqb1712J78ZXpLEvjbALAacLPrrvcJxta6XpU8Cd6F8NUHqBGd2Y4oWD9iQnyXB7umC72djzJFJVEgN5Z37DdiXPscqCMs7mX6qpuhq8thyKCDVhkvT9sr9t5EU7LYMxUHJgDdBS8K2GfTf76NTrHNV9AqjWcbbGM4EpPtGjsB8g6DjoH3xTdAtb9GE1PB2pFvucUMWrdT|j}
        handleChange={form.handleChange(PairingRequest)}
        error={form.getFieldError(Field(PairingRequest))}
        multiline=true
        numberOfLines=9
      />
      <View style=styles##verticalFormAction>
        <Buttons.SubmitPrimary
          text="CONNECT TO DAPP"
          onPress={_ => form.submit()}
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </View>
  </ModalFormView>;
};
