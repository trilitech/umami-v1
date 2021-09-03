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

module StateLenses = [%lenses type state = {name: string}];
module SecretCreateForm = ReForm.Make(StateLenses);

let checkExists = (secrets, values: StateLenses.state): ReSchema.fieldState =>
  secrets->Array.some((v: Secret.derived) => v.secret.name == values.name)
    ? Error(I18n.form_input_error#name_already_registered) : Valid;

[@react.component]
let make = (~secret: Secret.derived, ~closeAction) => {
  let secretsRequest = StoreContext.Secrets.useLoad();
  let (updateSecretRequest, updateSecret) = StoreContext.Secrets.useUpdate();
  let secrets = secretsRequest->ApiRequest.getWithDefault([||]);

  let action = secret => updateSecret(secret);

  let form: SecretCreateForm.api =
    SecretCreateForm.use(
      ~schema={
        SecretCreateForm.Validation.(
          Schema(nonEmpty(Name) + custom(checkExists(secrets), Name))
        );
      },
      ~onSubmit=
        ({state}) => {
          let {index, secret} = secret;
          action(
            Secret.{
              index,
              secret: {
                ...secret,
                name: state.values.name,
              },
            },
          )
          ->FutureEx.getOk(() => closeAction());

          None;
        },
      ~initialState={name: secret.secret.name},
      ~i18n=FormUtils.i18n,
      (),
    );

  let onSubmit = _ => {
    form.submit();
  };

  let loading = updateSecretRequest->ApiRequest.isLoading;

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      I18n.title#secret_update->React.string
    </Typography.Headline>
    <FormGroupTextInput
      label=I18n.label#account_create_name
      value={form.values.name}
      handleChange={form.handleChange(Name)}
      error={form.getFieldError(Field(Name))}
    />
    <Buttons.SubmitPrimary
      text=I18n.btn#update
      onPress=onSubmit
      loading
      style=FormStyles.formSubmit
      disabledLook={!formFieldsAreValids}
    />
  </ModalFormView>;
};
