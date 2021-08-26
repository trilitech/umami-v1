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

module StateLenses = [%lenses
  type state = {
    password: string,
    confirmPassword: string,
  }
];

module CreatePasswordForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "wordsList": style(~flexDirection=`row, ~flexWrap=`wrap, ()),
      "wordItem":
        style(
          ~marginVertical=2.->dp,
          ~flexGrow=1.,
          ~flexShrink=1.,
          ~flexBasis=40.->pct,
          (),
        ),
      "wordSpacer": style(~width=20.->dp, ()),
    })
  );

let isConfirmPassword = (values: StateLenses.state) => {
  let fieldState: ReSchema.fieldState =
    values.confirmPassword == values.password
      ? Valid : Error(I18n.form_input_error#confirm_password);
  fieldState;
};

let passwordLengthCheck = (values: StateLenses.state) => {
  let fieldState: ReSchema.fieldState =
    values.password->Js.String.length >= 8
      ? Valid : Error(I18n.form_input_error#password_length);
  fieldState;
};

[@react.component]
let make =
    (
      ~mnemonic: array(string),
      ~derivationPath,
      ~createSecretWithMnemonic,
      ~loading,
    ) => {
  let secrets = StoreContext.Secrets.useGetAll();

  let existingNonLedgerSecretsCount =
    secrets->Array.keep(s => s.secret.kind != Ledger)->Array.length;

  let displayConfirmPassword = existingNonLedgerSecretsCount < 1;
  let form: CreatePasswordForm.api =
    CreatePasswordForm.use(
      ~validationStrategy=OnDemand,
      ~schema={
        CreatePasswordForm.Validation.(
          Schema(
            nonEmpty(Password)
            + (
              displayConfirmPassword
                ? custom(passwordLengthCheck, Password) : [||]
            )
            + (
              displayConfirmPassword
                ? custom(isConfirmPassword, ConfirmPassword) : [||]
            ),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          createSecretWithMnemonic(
            SecretApiRequest.{
              name:
                "Secret " ++ (existingNonLedgerSecretsCount + 1)->string_of_int,
              mnemonic,
              derivationPath,
              password: state.values.password,
            },
          );

          None;
        },
      ~initialState={password: "", confirmPassword: ""},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  let passwordPlaceholder =
    displayConfirmPassword
      ? I18n.input_placeholder#enter_new_password
      : I18n.input_placeholder#enter_password;

  <>
    <FormGroupTextInput
      label=I18n.label#password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={form.getFieldError(Field(Password))}
      placeholder=passwordPlaceholder
      textContentType=`password
      secureTextEntry=true
    />
    {displayConfirmPassword
       ? <FormGroupTextInput
           label=I18n.label#confirm_password
           value={form.values.confirmPassword}
           handleChange={form.handleChange(ConfirmPassword)}
           error={form.getFieldError(Field(ConfirmPassword))}
           placeholder=I18n.input_placeholder#confirm_password
           textContentType=`password
           secureTextEntry=true
         />
       : React.null}
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.btn#finish
        onPress=onSubmit
        loading
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </>;
};
