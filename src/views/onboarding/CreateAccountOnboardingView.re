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

  // using a react ref prevent from genereting other mnemonic at other render
  // a useState can also be used, but because we don't need to set
  // other value later, it's unecessary to be used
  let mnemonic =
    React.useRef(Bip39.generate(256)->Js.String2.split(" ")).current;

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
           {I18n.t#stepof(1, 3)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.t#account_create_record_recovery->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.expl#account_create_record_recovery->React.string
         </Typography.Body2>
         <MnemonicListView mnemonic />
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
           {I18n.t#stepof(2, 3)->React.string}
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
       <>
         <Typography.Overline3
           colorStyle=`highEmphasis style=styles##stepPager>
           {I18n.t#stepof(3, 3)->React.string}
         </Typography.Overline3>
         <Typography.Overline1 style=styles##stepTitle>
           I18n.title#account_create_password->React.string
         </Typography.Overline1>
         <Typography.Body2 colorStyle=`mediumEmphasis style=styles##stepBody>
           I18n.expl#account_create_password_not_recorded->React.string
         </Typography.Body2>
         <CreatePasswordView
           mnemonic
           onPressCancel={_ => setFormStep(_ => Step2)}
           createSecretWithMnemonic
           loading
         />
       </>
     }}
  </ModalFormView>;
};
