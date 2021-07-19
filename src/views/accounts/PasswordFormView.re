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

module StateLenses = [%lenses type state = {password: string}];

module PasswordForm = ReForm.Make(StateLenses);

let usePasswordForm = submitPassword => {
  let form: PasswordForm.api =
    PasswordForm.use(
      ~schema={
        PasswordForm.Validation.(Schema(nonEmpty(Password)));
      },
      ~onSubmit=
        ({state, raiseSubmitFailed}) => {
          submitPassword(~password=state.values.password)
          ->Future.tapError(
              fun
              | ErrorHandler.Taquito(WrongPassword) =>
                raiseSubmitFailed(Some(I18n.form_input_error#wrong_password))
              | _ => (),
            )
          ->ignore;
          None;
        },
      ~initialState={password: ""},
      ~i18n=FormUtils.i18n,
      (),
    );
  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);
  (form, formFieldsAreValids);
};

module PasswordField = {
  [@react.component]
  let make = (~form: PasswordForm.api) => {
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={
        [
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Password)),
        ]
        ->UmamiCommon.Lib.Option.firstSome
      }
      textContentType=`password
      secureTextEntry=true
      onSubmitEditing={_event => {form.submit()}}
    />;
  };
};

[@react.component]
let make =
    (
      ~loading=false,
      ~submitPassword: (~password: string) => Future.t(Result.t(_)),
    ) => {
  let (form, formFieldsAreValids) = usePasswordForm(submitPassword);

  <>
    <PasswordField form />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.btn#confirm
        onPress={_event => {form.submit()}}
        loading
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
