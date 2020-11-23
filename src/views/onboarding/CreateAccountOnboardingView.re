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
      "formAction":
        style(
          ~marginTop=28.->dp,
          ~flexDirection=`row,
          ~justifyContent=`center,
          (),
        ),
      "loadingView":
        style(
          ~height=400.->dp,
          ~justifyContent=`center,
          ~alignItems=`center,
          (),
        ),
    })
  );

[@react.component]
let make = (~onPressCancel) => {
  let (formStep, setFormStep) = React.useState(_ => Step1);

  let (accountWithMnemonicRequest, createAccountWithMnemonic) =
    AccountApiRequest.useCreateWithMnemonics();

  let refreshAccounts = StoreContext.useRefreshAccounts();
  let handleAdd = () => refreshAccounts(~loading=false, ())->ignore;
  let createAccountWithMnemonic = p =>
    createAccountWithMnemonic(p)->Future.tapOk(_ => handleAdd())->ignore;

  // using a react ref prevent from genereting other mnemonic at other render
  // a useState can also be used, but because we don't need to set
  // other value later, it's unecessary to be used
  let mnemonic =
    React.useRef(Bip39.generate(256)->Js.String2.split(" ")).current;

  <ModalView.Form>
    {switch (accountWithMnemonicRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.Headline2 style=styles##title>
           "Account created"->React.string
         </Typography.Headline2>
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Done(Error(error)) =>
       <>
         <ErrorView error />
         <View style=styles##formAction>
           <FormButton text="OK" onPress=onPressCancel />
         </View>
       </>
     | Loading =>
       <View style=styles##loadingView>
         <ActivityIndicator
           animating=true
           size=ActivityIndicator_Size.large
           color="#FFF"
         />
       </View>
     | NotAsked =>
       <>
         <Typography.Headline2 style=styles##title>
           "Create new account"->React.string
         </Typography.Headline2>
         {switch (formStep) {
          | Step1 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                "Step 1 of 3"->React.string
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                "Record your recovery phrase"->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                {js|Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline.|js}
                ->React.string
              </Typography.Body3>
              <MnemonicListView mnemonic />
              <View style=styles##formAction>
                <FormButton text="CANCEL" onPress=onPressCancel />
                <FormButton
                  text={js|OK, I’VE RECORDED IT|js}
                  onPress={_ => setFormStep(_ => Step2)}
                />
              </View>
            </>
          | Step2 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                "Step 2 of 3"->React.string
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                "Verify your recovery phrase"->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                {js|We will now verify that you’ve properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number.|js}
                ->React.string
              </Typography.Body3>
              <VerifyMnemonicView
                mnemonic
                onPressCancel
                goNextStep={_ => setFormStep(_ => Step3)}
              />
            </>
          | Step3 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                "Step 3 of 3"->React.string
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                "Set a password to secure your wallet"->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                {js|Please note that this password is not recorded anywhere and only applies to this machine.|js}
                ->React.string
              </Typography.Body3>
              <CreatePasswordView
                mnemonic
                onPressCancel
                createAccountWithMnemonic
              />
            </>
          }}
       </>
     }}
  </ModalView.Form>;
};
