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

module StateLenses = [%lenses type state = {selectedBackupFile: string}];

module SelectBackupFileForm = ReForm.Make(StateLenses);

let styles =
  Style.(
    StyleSheet.create({
      "inputTypeButton": style(~flexDirection=`row, ~alignItems=`center, ()),
      "inputTypeText": style(~lineHeight=16., ~marginLeft=7.->dp, ()),
    })
  );

let form = (~backupFile, ~setBackupFile, ~next) =>
  SelectBackupFileForm.use(
    ~validationStrategy=OnDemand,
    ~schema={
      SelectBackupFileForm.Validation.(Schema(nonEmpty(SelectedBackupFile)));
    },
    ~onSubmit=
      ({state}) => {
        setBackupFile(_ =>
          Some(System.Path.mk(state.values.selectedBackupFile))
        );
        next(state);
        None;
      },
    ~initialState={
      selectedBackupFile:
        backupFile->Option.mapWithDefault("", System.Path.toString),
    },
    ~i18n=FormUtils.i18n,
    (),
  );

module FileField = {
  [@react.component]
  let make = (~form: SelectBackupFileForm.api) => {
    let theme = ThemeContext.useTheme();

    <FormGroupTextInput
      label=I18n.Label.storage_location
      value={form.values.selectedBackupFile}
      handleChange={form.handleChange(SelectedBackupFile)}
      error={form.getFieldError(Field(SelectedBackupFile))}
      placeholder=I18n.Input_placeholder.select_backup_path
      rightView={
        <TouchableOpacity
          style=styles##inputTypeButton
          onPress={_ =>
            remote.dialog
            ->Dialog.Save.show
            ->Promise.fromJs(_ => ())
            ->Promise.getOk(obj =>
                form.handleChange(
                  SelectedBackupFile,
                  obj.filePath->Option.getWithDefault(""),
                )
              )
          }>
          <Icons.Folder size=14. color={theme.colors.iconPrimary} />
          <Typography.ButtonSecondary
            style=Style.(
              array([|
                styles##inputTypeText,
                style(~color=theme.colors.textPrimary, ()),
              |])
            )>
            I18n.Btn.browse_for_folder->React.string
          </Typography.ButtonSecondary>
        </TouchableOpacity>
      }
    />;
  };
};

[@react.component]
let make = (~backupFile, ~setBackupFile, ~goNextStep) => {
  let form: SelectBackupFileForm.api =
    form(~backupFile, ~setBackupFile, ~next=goNextStep);

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  <>
    <FileField form />
    <View style=FormStyles.verticalFormAction>
      <Buttons.SubmitPrimary
        text=I18n.Btn.continue
        onPress={_ => form.submit()}
        disabledLook={!formFieldsAreValids}
      />
      <Buttons.FormSecondary
        style=FormStyles.formSecondary
        text=I18n.Btn.setup_later
        fontSize=14.
        onPress={_ => goNextStep(form.state)}
      />
    </View>
  </>;
};
