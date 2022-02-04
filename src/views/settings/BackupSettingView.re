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

module StateLenses = [%lenses
  type state = {
    isEnabled: bool,
    selectedBackupFile: string,
  }
];

module BackupSettingForm = ReForm.Make(StateLenses);

let form = (~isEnabled, ~backupFile, ~setBackupFile) =>
  BackupSettingForm.use(
    ~validationStrategy=OnDemand,
    ~schema={
      BackupSettingForm.Validation.(
        Schema(true_(IsEnabled) + nonEmpty(SelectedBackupFile))
      );
    },
    ~onSubmit=
      ({state}) => {
        setBackupFile(_ =>
          state.values.isEnabled
            ? Some(System.Path.mk(state.values.selectedBackupFile)) : None
        );
        None;
      },
    ~initialState={
      isEnabled,
      selectedBackupFile:
        backupFile->Option.mapWithDefault("", System.Path.toString),
    },
    ~i18n=FormUtils.i18n,
    (),
  );

module BackupSwitch = {
  let styles =
    Style.(
      StyleSheet.create({"iconOffset": style(~paddingLeft=14.->dp, ())})
    );

  [@react.component]
  let make = (~isEnabled, ~toggleSwitch) => {
    let theme = ThemeContext.useTheme();

    let icon = (~color=?, ~style=?) =>
      isEnabled
        ? Icons.SwitchOn.build(
            ~style=Style.arrayOption([|style, styles##iconOffset->Some|]),
            ~color=theme.colors.iconPrimary,
          )
        : Icons.SwitchOff.build(
            ~style=Style.arrayOption([|style, styles##iconOffset->Some|]),
            ~color?,
          );

    <IconButton icon onPress=toggleSwitch iconSizeRatio=1.1 size=46. />;
  };
};

[@react.component]
let make = () => {
  let config = ConfigContext.useContent();
  let write = ConfigContext.useWrite();

  let (isEnabled, enable) = React.useState(_ => config.backupFile != None);

  React.useEffect1(
    _ => {
      if (!isEnabled) {
        write(configFile => {...configFile, backupFile: None});
      };
      None;
    },
    [|isEnabled|],
  );

  let setBackupFile = backupFile =>
    write(configFile =>
      {...configFile, backupFile: backupFile(config.backupFile)}
    );

  let form: BackupSettingForm.api =
    form(~isEnabled, ~backupFile=config.backupFile, ~setBackupFile);

  let addLog = LogsContext.useAdd();
  let didMountRef = React.useRef(false);

  React.useEffect1(
    _ => {
      if (didMountRef.current) { // do not force backup for a first render
        if (config.backupFile == None) {
          form.handleChange(SelectedBackupFile, "");
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
    [|config|],
  );

  let onSubmit = _ => form.submit();

  let formFieldsAreValids =
    FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields);

  let theme = ThemeContext.useTheme();

  <Block title=I18n.Settings.backup_title>
    <View style=Style.(style(~flex=1., ()))>
      <View accessibilityRole=`form style=styles##row>
        <ColumnLeft
          style=Style.(style(~minWidth=38.->dp, ~maxWidth=38.->dp, ()))>
          <BackupSwitch
            isEnabled={form.values.isEnabled}
            toggleSwitch={_ => {
              enable(previousState => !previousState);
              form.handleChange(IsEnabled, !isEnabled);
            }}
          />
        </ColumnLeft>
        <ColumnRight
          style=Style.(
            style(~flexGrow=99., ~flexShrink=99., ~flexBasis=0.->dp, ())
          )>
          <Typography.Body1>
            I18n.Settings.backup_text->React.string
          </Typography.Body1>
        </ColumnRight>
      </View>
      {isEnabled
         ? <>
             <View style=Style.(style(~height=16.->dp, ())) />
             <View accessibilityRole=`form style=styles##row>
               <ColumnLeft
                 style=Style.(style(~minWidth=38.->dp, ~maxWidth=38.->dp, ()))>
                 <View />
               </ColumnLeft>
               <ColumnRight
                 style=Style.(
                   style(
                     ~flexGrow=99.,
                     ~flexShrink=99.,
                     ~flexBasis=0.->dp,
                     (),
                   )
                 )>
                 <SettingFormGroupTextInput
                   label=I18n.Label.storage_location
                   rightView={
                     <TouchableOpacity
                       style=styles##browseButton
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
                       <Icons.Folder
                         size=14.
                         color={theme.colors.iconPrimary}
                       />
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
                   value={form.values.selectedBackupFile}
                   onValueChange={form.handleChange(SelectedBackupFile)}
                   error={form.getFieldError(Field(SelectedBackupFile))}
                   placeholder=I18n.Input_placeholder.select_backup_path
                   onSubmitEditing=onSubmit
                 />
               </ColumnRight>
               <ColumnRight
                 style=Style.(
                   style(~minWidth=104.->dp, ~maxWidth=104.->dp, ())
                 )>
                 <Buttons.SubmitPrimary
                   style=styles##saveButton
                   text=I18n.Btn.save
                   onPress=onSubmit
                   disabledLook={!formFieldsAreValids}
                 />
               </ColumnRight>
             </View>
           </>
         : <View />}
    </View>
  </Block>;
};
