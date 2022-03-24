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
open SettingsComponents;
open Remote;

let styles =
  Style.(
    StyleSheet.create({
      "row": style(~flex=1., ~flexDirection=`row, ~alignItems=`center, ()),
      "input": style(~alignItems=`center, ()),
      "browseButton":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~marginTop=2.->dp,
          (),
        ),
      "browseButtonText": style(~lineHeight=16., ~marginLeft=7.->dp, ()),
      "saveButton":
        style(
          ~width=104.->dp,
          ~height=34.->dp,
          ~marginLeft=16.->dp,
          ~marginTop=6.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let config = ConfigContext.useContent();
  let write = ConfigContext.useWrite();

  let (isEnabled, enable) = React.useState(() => config.backupFile != None);

  let (backupFile, setBackupFile) = React.useState(() => "");

  React.useEffect1(
    _ => {
      if (!isEnabled) {
        write(configFile => {...configFile, backupFile: None});
      };
      None;
    },
    [|isEnabled|],
  );

  let setBackupFileConfig = backupFile =>
    write(configFile =>
      {...configFile, backupFile: backupFile(config.backupFile)}
    );

  let addLog = LogsContext.useAdd();
  let didMountRef = React.useRef(false);

  React.useEffect1(
    _ => {
      if (didMountRef.current) {
        // do not force backup for a first render
        if (config.backupFile == None) {
          setBackupFile(_ => "");
        } else {
          switch (WalletAPI.Accounts.forceBackup(~config)) {
          | Ok(_) =>
            addLog(
              true,
              Logs.info(
                ~origin=Logs.Settings,
                I18n.Settings.backup_path_saved,
              ),
            )
          | Error(error) =>
            addLog(true, Logs.error(~origin=Logs.Settings, error))
          };
        };
      } else {
        didMountRef.current = true;
      };
      None;
    },
    [|config.backupFile|],
  );

  let onSubmit = _ =>
    setBackupFileConfig(_ =>
      isEnabled && backupFile != "" ? Some(System.Path.mk(backupFile)) : None
    );

  let theme = ThemeContext.useTheme();

  let onPressBrowse = _ =>
    remote.dialog
    ->Dialog.Save.show
    ->Promise.fromJs(_ => ())
    ->Promise.getOk(obj =>
        setBackupFile(_ => obj.filePath->Option.getWithDefault(""))
      );

  let error = backupFile == "" ? I18n.Form_input_error.mandatory->Some : None;

  let browse = () =>
    <View style=styles##row>
      <SettingFormGroupTextInput
        label=I18n.Label.storage_location
        rightView={
          <TouchableOpacity style=styles##browseButton onPress=onPressBrowse>
            <Icons.Folder size=14. color={theme.colors.iconPrimary} />
            <Typography.ButtonSecondary
              style=Style.(
                array([|
                  styles##browseButtonText,
                  style(~color=theme.colors.textPrimary, ()),
                |])
              )>
              I18n.Btn.browse_for_folder->React.string
            </Typography.ButtonSecondary>
          </TouchableOpacity>
        }
        value=backupFile
        onValueChange={v => setBackupFile(_ => v)}
        error
        placeholder=I18n.Input_placeholder.select_backup_path
        onSubmitEditing=onSubmit
      />
      <Buttons.SubmitPrimary
        style=styles##saveButton
        text=I18n.Btn.save
        onPress=onSubmit
        disabledLook={error != None}
      />
    </View>;

  <Block title=I18n.Settings.backup_title>
    <View style=Style.(style(~flex=1., ()))>
      <SwitchItem
        label=I18n.Settings.backup_text
        value=isEnabled
        setValue={_ => {enable(previousState => !previousState)}}
      />
      {isEnabled ? browse() : React.null}
    </View>
  </Block>;
};
