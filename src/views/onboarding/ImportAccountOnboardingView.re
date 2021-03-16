open ReactNative;

type step =
  | Step1
  | Step2;

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
let make = (~closeAction) => {
  let (formStep, setFormStep) = React.useState(_ => Step1);

  let (secretWithMnemonicRequest, createSecretWithMnemonic) =
    StoreContext.Secrets.useCreateWithMnemonics();

  let addLog = LogsContext.useAdd();

  let createSecretWithMnemonic = p =>
    createSecretWithMnemonic(p)
    ->Future.tapOk(_ => {closeAction()})
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ =>
        I18n.t#account_created
      )
    ->ignore;

  let (mnemonic, setMnemonic) = React.useState(_ => Array.make(24, "zebra"));

  let loading = secretWithMnemonicRequest->ApiRequest.isLoading;

  <ModalFormView>
    <Typography.Headline style=styles##title>
      I18n.title#import_account->React.string
    </Typography.Headline>
    {switch (formStep) {
     | Step1 =>
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
           onPressCancel={_ => closeAction()}
           goNextStep={_ => setFormStep(_ => Step2)}
         />
       </>
     | Step2 =>
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(2, 2)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.title#account_create->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.expl#account_create_password_not_recorded->React.string
         </Typography.Body2>
         <CreatePasswordView
           mnemonic
           onPressCancel={_ => setFormStep(_ => Step1)}
           createSecretWithMnemonic
           loading
         />
       </>
     }}
  </ModalFormView>;
};
