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
open Remote;

module StateLenses = [%lenses
  type state = {
    selectedBackupFile: string,
    password: string,
  }
];

module RestoreForm = ReForm.Make(StateLenses);

let useRestoreForm = submit => {
  let form: RestoreForm.api =
    RestoreForm.use(
      ~schema={
        RestoreForm.Validation.(
          Schema(nonEmpty(SelectedBackupFile) + nonEmpty(Password))
        );
      },
      ~onSubmit=
        ({state, raiseSubmitFailed}) => {
          submit(
            ~selectedBackupFile=
              System.Path.mk(state.values.selectedBackupFile),
            ~password=state.values.password,
          )
          ->Promise.getError(
              fun
              | SecureStorage.WrongPassword
              | SecureStorage.Cipher.DecryptError =>
                raiseSubmitFailed(Some(I18n.Form_input_error.wrong_password))
              | _ => (),
            );

          None;
        },
      ~initialState={selectedBackupFile: "", password: ""},
      ~i18n=FormUtils.i18n,
      (),
    );
  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);
  (form, formFieldsAreValids);
};

module FileField = {
  let styles =
    Style.(
      StyleSheet.create({
        "inputTypeButton":
          style(~flexDirection=`row, ~alignItems=`center, ()),
        "inputTypeText": style(~lineHeight=16., ()),
      })
    );

  [@react.component]
  let make = (~form: RestoreForm.api) => {
    let theme = ThemeContext.useTheme();

    <FormGroupTextInput
      label=I18n.Label.file
      value={form.values.selectedBackupFile}
      handleChange={form.handleChange(SelectedBackupFile)}
      error={form.getFieldError(Field(SelectedBackupFile))}
      placeholder=I18n.Input_placeholder.select_backup_path
      rightView={
        <TouchableOpacity
          style=styles##inputTypeButton
          onPress={_ =>
            remote.dialog
            ->Dialog.Open.show
            ->Promise.fromJs(_ => ())
            ->Promise.getOk(obj =>
                form.handleChange(
                  SelectedBackupFile,
                  obj.filePaths[0]->Option.getWithDefault(""),
                )
              )
          }>
          <Icons.ArrowUp size=24. color={theme.colors.iconPrimary} />
          <Typography.ButtonSecondary
            style=Style.(
              array([|
                styles##inputTypeText,
                style(~color=theme.colors.textPrimary, ()),
              |])
            )>
            I18n.Btn.upload_file->React.string
          </Typography.ButtonSecondary>
        </TouchableOpacity>
      }
    />;
  };
};

module PasswordField = {
  [@react.component]
  let make = (~form: RestoreForm.api) => {
    <FormGroupTextInput
      label=I18n.Label.password
      value={form.values.password}
      handleChange={form.handleChange(Password)}
      error={
        [
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(Password)),
        ]
        ->Option.firstSome
      }
      textContentType=`password
      secureTextEntry=true
      onSubmitEditing={_event => {form.submit()}}
    />;
  };
};

[@react.component]
let make = (~closeAction) => {
  let (secretsFromBackupFileRequest, createSecretsFromBackupFile) =
    StoreContext.Secrets.useCreateFromBackupFile();

  let write = ConfigContext.useWrite();

  let (form, formFieldsAreValids) =
    useRestoreForm((~selectedBackupFile, ~password) => {
      let secret =
        SecretApiRequest.{backupFile: selectedBackupFile, password};

      createSecretsFromBackupFile(secret)
      ->Promise.tapOk(_ => {
          write(configFile =>
            {...configFile, backupFile: Some(selectedBackupFile)}
          );
          closeAction();
        });
    });

  let loading = secretsFromBackupFileRequest->ApiRequest.isLoading;

  let closing =
    ModalFormView.confirm(~actionText=I18n.Btn.cancel, closeAction);

  <ModalFormView closing>
    <Typography.Headline style=FormStyles.onboarding##title>
      I18n.Title.restore_account->React.string
    </Typography.Headline>
    <Typography.Overline1 style=FormStyles.onboarding##stepBody>
      I18n.Title.select_backup_file->React.string
    </Typography.Overline1>
    <FileField form />
    <PasswordField form />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.Btn.access_wallet
        onPress={_ => form.submit()}
        loading
        disabledLook={!formFieldsAreValids}
      />
    </View>
  </ModalFormView>;
};
