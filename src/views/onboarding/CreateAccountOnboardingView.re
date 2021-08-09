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
  | Step4;

[@react.component]
let make = (~closeAction) => {
  let (formStep, setFormStep) = React.useState(_ => Step1);

  let (secretWithMnemonicRequest, createSecretWithMnemonic) =
    StoreContext.Secrets.useCreateWithMnemonics();

  let secrets = StoreContext.Secrets.useGetAll();
  let existingSecretsCount = secrets->Array.length;
  let noExistingPassword = existingSecretsCount < 1;

  let addLog = LogsContext.useAdd();

  let settings = SdkContext.useSettings();

  let createSecretWithMnemonic = p =>
    System.Client.initDir(settings->AppSettings.baseDir)
    ->Future.flatMapOk(() =>
        createSecretWithMnemonic(p)->Future.mapError(ErrorHandler.toString)
      )
    ->Future.tapOk(_ => {closeAction()})
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#account_created
      )
    ->ignore;

  // using a react ref prevent from genereting other mnemonic at other render
  // a useState can also be used, but because we don't need to set
  // other value later, it's unecessary to be used
  let mnemonic =
    React.useRef(Bip39.generate(256)->Js.String2.split(" ")).current;
  let (derivationPath, setDerivationPath) =
    React.useState(_ => DerivationPath.Pattern.(default->fromTezosBip44));

  let loading = secretWithMnemonicRequest->ApiRequest.isLoading;

  <ModalFormView>
    <Typography.Headline style=styles##title>
      I18n.title#account_create->React.string
    </Typography.Headline>
    {switch (formStep) {
     | Step1 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(1, 4)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.t#account_create_record_recovery->React.string
         </Typography.Overline1>
         <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
           <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
             I18n.expl#account_create_record_recovery->React.string
           </Typography.Body2>
           <MnemonicListView mnemonic />
         </DocumentContext.ScrollView>
         <View style=FormStyles.formActionSpaceBetween>
           <Buttons.Form text=I18n.btn#back onPress={_ => closeAction()} />
           <Buttons.SubmitPrimary
             text=I18n.btn#create_account_record_ok
             onPress={_ => setFormStep(_ => Step2)}
           />
         </View>
       </>
     | Step2 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(2, 4)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.title#account_create_verify_phrase->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.expl#account_create_record_verify->React.string
         </Typography.Body2>
         <VerifyMnemonicView
           mnemonic
           onPressCancel={_ => setFormStep(_ => Step1)}
           goNextStep={_ => setFormStep(_ => Step3)}
         />
       </>
     | Step3 =>
       let subtitle = I18n.title#account_derivation_path;

       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(2, 3)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.expl#account_select_derivation_path->React.string
          </Typography.Body2>}
         <SelectDerivationPathView
           derivationPath
           setDerivationPath
           onPressCancel={_ => setFormStep(_ => Step2)}
           goNextStep={_ => setFormStep(_ => Step4)}
         />
       </>;
     | Step4 =>
       let subtitle =
         noExistingPassword
           ? I18n.title#account_create_password
           : I18n.title#account_enter_password;

       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(4, 4)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.expl#account_create_password_not_recorded->React.string
          </Typography.Body2>
          ->ReactUtils.onlyWhen(noExistingPassword)}
         <CreatePasswordView
           mnemonic
           derivationPath
           onPressCancel={_ => setFormStep(_ => Step2)}
           createSecretWithMnemonic
           loading
           existingSecretsCount
         />
       </>;
     }}
  </ModalFormView>;
};
