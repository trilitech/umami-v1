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
    AccountApiRequest.useCreateAccountWithMnemonic();

  let (mnemonic, setMnemonic) = React.useState(_ => Belt.Array.make(24, ""));

  <ModalView>
    {switch (accountWithMnemonicRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.Headline2 style=styles##title>
           "Account imported"->React.string
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
           "Import account"->React.string
         </Typography.Headline2>
         {switch (formStep) {
          | Step1 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                "Step 1 of 2"->React.string
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                "Enter your recovery phrase"->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                {js|Please fill in the recovery phrase in sequence.|js}
                ->React.string
              </Typography.Body3>
              <FillMnemonicView
                mnemonic
                setMnemonic
                onPressCancel
                goNextStep={_ => setFormStep(_ => Step2)}
              />
            </>
          | Step2 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                "Step 2 of 2"->React.string
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
  </ModalView>;
};
