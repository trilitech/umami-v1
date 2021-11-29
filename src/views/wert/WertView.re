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
  let defaultInit = (account: option(Account.t)) =>
    WertForm.StateLenses.{recipient: account};

  let use = (~initValues=?, initAccount, onSubmit) => {
    WertForm.use(
      ~schema={
        WertForm.Validation.(
          Schema(
            custom(values => values.recipient->FormUtils.notNone, Recipient),
          )
        );
      },
      ~onSubmit=
        f => {
          onSubmit(f);
          None;
        },
      ~initialState=
        initValues->Option.getWithDefault(defaultInit(initAccount)),
      ~i18n=FormUtils.i18n,
      (),
    );
  };

  module View = {
    open WertForm;

    [@react.component]
    let make = (~form, ~loading) => {
      let formFieldsAreValids =
        FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

      <>
        <View style=FormStyles.header>
          <Typography.Overline1>
            I18n.title#wert_recipient->React.string
          </Typography.Overline1>
        </View>
        <FormGroupAccountSelector
          label=I18n.label#send_recipient
          value={form.values.recipient}
          handleChange={form.handleChange(Recipient)}
          error={form.getFieldError(Field(Recipient))}
        />
        <View style=FormStyles.verticalFormAction>
          <Buttons.SubmitPrimary
            text=I18n.btn#buy_tez
            onPress={_ => form.submit()}
            loading
            disabledLook={!formFieldsAreValids}
          />
        </View>
      </>;
    };
  };
};

type step =
  | Disclaimer
  | SelectRecipient;

let stepToString = step =>
  switch (step) {
  | Disclaimer => "disclaimer"
  | SelectRecipient => "selectrecipient"
  };

[@react.component]
let make = (~submit, ~closeAction) => {
  let account = StoreContext.SelectedAccount.useGet();

  let (modalStep, setModalStep) =
    React.useState(_ =>
      WertDisclaimer.needSigning() ? Disclaimer : SelectRecipient
    );

  let onSubmit = ({state, _}: WertForm.onSubmitAPI) =>
    switch (state.values.recipient) {
    | Some(account) =>
      submit(account.address);
    | None => ()
    };

  let form: WertForm.api = Form.use(account, onSubmit);

  let title =
    switch (modalStep) {
    | Disclaimer => I18n.title#notice
    | SelectRecipient => I18n.title#buy_tez
    };

  let back = None;
  let closing = Some(ModalFormView.Close(closeAction));

  <ModalFormView title back ?closing>
    {switch (modalStep) {
     | Disclaimer =>
       <WertDisclaimerView
         onSign={unsigned =>
           if (!unsigned) {
             WertDisclaimer.sign();
             setModalStep(_ => SelectRecipient);
           }
         }
       />
     | SelectRecipient => <Form.View form loading=false />
     }}
  </ModalFormView>;
};
