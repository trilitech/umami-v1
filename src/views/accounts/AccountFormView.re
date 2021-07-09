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

module Generic = {
  module StateLenses = [%lenses
    type state = {
      name: string,
      secret: string,
    }
  ];
  module AccountCreateForm = ReForm.Make(StateLenses);

  type action =
    | Create
    | Edit(Account.t);

  let aliasCheckExists = (aliases, values: StateLenses.state) => {
    AliasHelpers.formCheckExists(aliases, values.name);
  };

  [@react.component]
  let make =
      (
        ~init,
        ~buttonText,
        ~action: (~name: string, ~secretIndex: int) => unit,
        ~request,
        ~secret: option(Secret.derived)=?,
        ~hideSecretSelector=false,
      ) => {
    let aliasesRequest = StoreContext.Aliases.useRequest();
    let aliases =
      aliasesRequest
      ->ApiRequest.getDoneOk
      ->Option.getWithDefault(Map.String.empty);

    let form: AccountCreateForm.api =
      AccountCreateForm.use(
        ~schema={
          AccountCreateForm.Validation.(
            Schema(
              nonEmpty(Name) + custom(aliasCheckExists(aliases), Name),
            )
          );
        },
        ~onSubmit=
          ({state}) => {
            action(
              ~name=state.values.name,
              ~secretIndex=
                state.values.secret->Js.Float.fromString->int_of_float,
            );
            None;
          },
        ~initialState={
          name: init,
          secret:
            secret->Option.mapWithDefault("", secret =>
              secret.index->string_of_int
            ),
        },
        ~i18n=FormUtils.i18n,
        (),
      );

    let onSubmit = _ => {
      form.submit();
    };

    let loading = request->ApiRequest.isLoading;

    let formFieldsAreValids =
      FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

    <>
      {hideSecretSelector
         ? React.null
         : <FormGroupSecretSelector
             label=I18n.label#account_secret
             value={
               form.values.secret == "" ? None : Some(form.values.secret)
             }
             handleChange={secret =>
               form.handleChange(Secret, secret.Secret.index->Int.toString)
             }
             error={form.getFieldError(Field(Secret))}
             disabled={secret->Option.isSome}
           />}
      <FormGroupTextInput
        label=I18n.label#account_create_name
        value={form.values.name}
        handleChange={form.handleChange(Name)}
        placeholder=I18n.input_placeholder#add_accounts_name
        error={form.getFieldError(Field(Name))}
      />
      <Buttons.SubmitPrimary
        text=buttonText
        onPress=onSubmit
        loading
        style=FormStyles.formSubmit
        disabledLook={!formFieldsAreValids}
      />
    </>;
  };
};

module Update = {
  [@react.component]
  let make = (~closeAction, ~account: Account.t) => {
    let (updateAccountRequest, updateAccount) =
      StoreContext.Accounts.useUpdate();

    let addLog = LogsContext.useAdd();

    let action = (~name as new_name, ~secretIndex as _s) => {
      updateAccount({old_name: account.name, new_name})
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_updated
        )
      ->Future.tapOk(() => closeAction())
      ->ignore;
    };

    <ModalFormView closing={ModalFormView.Close(closeAction)}>
      <Typography.Headline style=FormStyles.header>
        I18n.title#account_update->React.string
      </Typography.Headline>
      <Generic
        init={account.name}
        buttonText=I18n.btn#update
        request=updateAccountRequest
        action
        hideSecretSelector=true
      />
    </ModalFormView>;
  };
};

module Create = {
  [@react.component]
  let make = (~closeAction, ~secret: option(Secret.derived)=?) => {
    let (createDeriveRequest, deriveAccount) =
      StoreContext.Secrets.useDerive();

    let addLog = LogsContext.useAdd();

    let (formValues, setFormValues) = React.useState(_ => None);

    let action = (~name, ~secretIndex, ~password) => {
      deriveAccount({name, index: secretIndex, password})
      ->Future.mapOk(_ => ())
      ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
          I18n.t#account_created
        )
      ->Future.tapOk(() => closeAction());
    };

    <ModalFormView closing={ModalFormView.Close(closeAction)}>
      <Typography.Headline style=FormStyles.header>
        I18n.title#derive_account->React.string
      </Typography.Headline>
      {switch (formValues) {
       | None =>
         <Generic
           init=""
           buttonText=I18n.btn#add
           request=createDeriveRequest
           action={(~name, ~secretIndex) =>
             setFormValues(_ => Some((name, secretIndex)))
           }
           ?secret
         />
       | Some((name, secretIndex)) =>
         <PasswordFormView
           loading={createDeriveRequest->ApiRequest.isLoading}
           submitPassword={action(~name, ~secretIndex)}
         />
       }}
    </ModalFormView>;
  };
};
