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

let styles = FormStyles.onboarding;

type step =
  | MnemonicsStep
  | DerivationPathStep
  | PasswordStep(step);

[@react.component]
let make = (~closeAction) => {
  let (formStep, setFormStep) = React.useState(_ => MnemonicsStep);

  let (secretWithMnemonicRequest, createSecretWithMnemonic) =
    StoreContext.Secrets.useCreateWithMnemonics();

  let (totalPages, setTotalPages) = React.useState(_ => 2);

  let secrets = StoreContext.Secrets.useGetAll();
  let existingSecretsCount = secrets->Array.length;
  let noExistingPassword = existingSecretsCount < 1;

  let config = ConfigContext.useContent();

  let addLog = LogsContext.useAdd();

  let createSecretWithMnemonic = p =>
    System.Client.initDir(config->ConfigUtils.baseDir)
    ->Future.mapError(System.File.Error.toString)
    ->Future.flatMapOk(() =>
        createSecretWithMnemonic(p)->Future.mapError(ErrorHandler.toString)
      )
    ->Future.tapOk(() => {closeAction()})
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#account_created
      )
    ->ignore;

  let (mnemonic, setMnemonic) = React.useState(_ => Array.make(24, ""));
  let formatState =
    React.useState(_ => FillMnemonicView.FormatSelector.Words24);
  let (derivationPath, setDerivationPath) =
    React.useState(_ => DerivationPath.Pattern.(default->fromTezosBip44));

  let loading = secretWithMnemonicRequest->ApiRequest.isLoading;

  let closing =
    ModalFormView.confirm(~actionText=I18n.btn#cancel, closeAction);

  let back =
    switch (formStep) {
    | MnemonicsStep => None
    | DerivationPathStep => Some(_ => setFormStep(_ => MnemonicsStep))
    | PasswordStep(prevStep) => Some(_ => setFormStep(_ => prevStep))
    };

  <ModalFormView closing back>
    <Typography.Headline style=styles##title>
      I18n.title#import_account->React.string
    </Typography.Headline>
    {switch (formStep) {
     | MnemonicsStep =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(1, 2)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.title#import_account_enter_phrase->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.expl#import_account_enter_phrase->React.string
         </Typography.Body2>
         <FillMnemonicView
           mnemonic
           setMnemonic
           formatState
           secondaryButton={(disabled, onPress) =>
             <Buttons.FormSecondary
               disabled
               text=I18n.btn#customize_derivation_path
               onPress={_ => {onPress()}}
             />
           }
           next={_ => setFormStep(_ => PasswordStep(MnemonicsStep))}
           nextSecondary={_ => {
             setFormStep(_ => DerivationPathStep);
             setTotalPages(_ => 3);
           }}
         />
       </>
     | DerivationPathStep =>
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
           goNextStep={_ =>
             setFormStep(_ => PasswordStep(DerivationPathStep))
           }
         />
       </>;
     | PasswordStep(_) =>
       let subtitle =
         noExistingPassword
           ? I18n.title#account_create_password
           : I18n.title#account_enter_password;

       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(totalPages, totalPages)->React.string}
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
           createSecretWithMnemonic
           loading
         />
       </>;
     }}
  </ModalFormView>;
};
