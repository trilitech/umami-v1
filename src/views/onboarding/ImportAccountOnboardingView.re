open ReactNative;

type step =
  | Step1
  | Step2
  | Step3;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "stepPager": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "stepTitle": style(~marginBottom=10.->dp, ~textAlign=`center, ()),
      "stepBody": style(~marginBottom=28.->dp, ~textAlign=`center, ()),
    })
  );

[@react.component]
let make = (~closeAction, ~existingSecretsCount=0) => {
  let (formStep, setFormStep) = React.useState(_ => Step1);

  let (secretWithMnemonicRequest, createSecretWithMnemonic) =
    StoreContext.Secrets.useCreateWithMnemonics();

  let settings = SdkContext.useSettings();

  let addLog = LogsContext.useAdd();

  let createSecretWithMnemonic = p =>
    System.Client.initDir(settings->AppSettings.baseDir)
    ->Future.flatMapOk(() => createSecretWithMnemonic(p))
    ->Future.tapOk(_ => {closeAction()})
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#account_created
      )
    ->ignore;

  let (mnemonic, setMnemonic) = React.useState(_ => Array.make(24, "zebra"));
  let (derivationScheme, setDerivationScheme) =
    React.useState(_ => "m/44'/1729'/?'/0'");

  let loading = secretWithMnemonicRequest->ApiRequest.isLoading;

  let displayConfirmPassword = existingSecretsCount < 1;

  <ModalFormView>
    <Typography.Headline style=styles##title>
      I18n.title#import_account->React.string
    </Typography.Headline>
    {switch (formStep) {
     | Step1 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(1, 3)->React.string}
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
           onPressCancel={_ => closeAction()}
           goNextStep={_ => setFormStep(_ => Step2)}
         />
       </>
     | Step2 =>
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
           derivationScheme
           setDerivationScheme
           onPressCancel={_ => setFormStep(_ => Step1)}
           goNextStep={_ => setFormStep(_ => Step3)}
         />
       </>;
     | Step3 =>
       let subtitle =
         displayConfirmPassword
           ? I18n.title#account_create_password
           : I18n.title#account_enter_password;

       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(3, 3)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           subtitle->React.string
         </Typography.Overline1>
         {<Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
            I18n.expl#account_create_password_not_recorded->React.string
          </Typography.Body2>
          ->ReactUtils.onlyWhen(displayConfirmPassword)}
         <CreatePasswordView
           mnemonic
           derivationScheme
           onPressCancel={_ => setFormStep(_ => Step2)}
           createSecretWithMnemonic
           loading
           existingSecretsCount
         />
       </>;
     }}
  </ModalFormView>;
};
