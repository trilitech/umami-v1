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

let styles = FormStyles.onboarding;

type step =
  | Step1
  | Step2
  | Step3
  | Step4
  | Step5;

[@react.component]
let make = (~closeAction) => {
  let (formStep, setFormStep) = React.useState(_ => Step1);

  let secrets = StoreContext.Secrets.useGetAll();
  let existingSecretsCount = secrets->Array.length;
  let noExistingPassword = existingSecretsCount == 0;
  let stepCount = noExistingPassword ? 5 : 4; // remove optional backup path selection if password exists

  // using a react ref prevent from genereting other mnemonic at other render
  // a useState can also be used, but because we don't need to set
  // other value later, it's unecessary to be used
  let mnemonic =
    React.useRef(Bip39.generate(256)->Js.String2.split(" ")).current;
  let (derivationPath, setDerivationPath) =
    React.useState(_ => DerivationPath.Pattern.(default->fromTezosBip44));
  let (backupFile, setBackupFile) = React.useState(_ => None);

  let closing =
    ModalFormView.confirm(~actionText=I18n.Btn.cancel, closeAction);

  let back =
    switch (formStep) {
    | Step1 => None
    | Step2 => Some(_ => setFormStep(_ => Step1))
    | Step3 => Some(_ => setFormStep(_ => Step2))
    | Step4 => Some(_ => setFormStep(_ => Step3))
    | Step5 => Some(_ => setFormStep(_ => noExistingPassword ? Step4 : Step3))
    };

  let writeConf = ConfigContext.useWrite();

  let onSubmit = _ => {
    if (noExistingPassword) {
      writeConf(config => {...config, backupFile});
    }
    closeAction();
  };

  <ModalFormView closing back>
    <Typography.Headline style=styles##title>
      I18n.Title.secret_create->React.string
    </Typography.Headline>
    {switch (formStep) {
     | Step1 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.stepof(1, stepCount)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.account_create_record_recovery->React.string
         </Typography.Overline1>
         <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
           <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
             I18n.Expl.secret_create_record_recovery->React.string
           </Typography.Body2>
           <MnemonicListView mnemonic />
         </DocumentContext.ScrollView>
         <View style=FormStyles.verticalFormAction>
           <Buttons.SubmitPrimary
             text=I18n.Btn.create_account_record_ok
             onPress={_ => setFormStep(_ => Step2)}
           />
         </View>
       </>
     | Step2 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.stepof(2, stepCount)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.Title.account_create_verify_phrase->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.Expl.secret_create_record_verify->React.string
         </Typography.Body2>
         <VerifyMnemonicView
           mnemonic
           goNextStep={_ => setFormStep(_ => Step3)}
         />
       </>
     | Step3 =>
       let subtitle = I18n.Title.account_derivation_path;

       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.stepof(3, stepCount)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.Expl.secret_select_derivation_path->React.string
          </Typography.Body2>}
         <SelectDerivationPathView
           derivationPath
           setDerivationPath
           goNextStep={_ =>
             setFormStep(_ => noExistingPassword ? Step4 : Step5)
           }
         />
       </>;
     | Step4 =>
       let subtitle = I18n.Title.account_backup_path;
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.optional_stepof(4, stepCount)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.Expl.secret_select_backup_path->React.string
          </Typography.Body2>}
         <SelectBackupPathView
           backupFile
           setBackupFile
           goNextStep={_ => setFormStep(_ => Step5)}
         />
       </>;
     | Step5 =>
       let subtitle =
         noExistingPassword
           ? I18n.Title.account_create_password
           : I18n.Title.account_enter_password;
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.stepof(stepCount, stepCount)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.Expl.secret_create_password_not_recorded->React.string
          </Typography.Body2>
          ->ReactUtils.onlyWhen(noExistingPassword)}
         <CreatePasswordView mnemonic derivationPath onSubmit />
       </>;
     }}
  </ModalFormView>;
};
