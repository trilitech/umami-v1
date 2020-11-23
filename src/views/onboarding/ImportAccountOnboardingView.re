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
    AccountApiRequest.useCreateWithMnemonics();

  let refreshAccounts = StoreContext.useRefreshAccounts();
  let handleAdd = () => refreshAccounts(~loading=false, ())->ignore;
  let createAccountWithMnemonic = p =>
    createAccountWithMnemonic(p)->Future.tapOk(_ => handleAdd())->ignore;

  let (mnemonic, setMnemonic) = React.useState(_ => Belt.Array.make(24, ""));

  <ModalView.Form>
    {switch (accountWithMnemonicRequest) {
     | Done(Ok(_result)) =>
       <>
         <Typography.Headline2 style=styles##title>
           I18n.t#account_imported->React.string
         </Typography.Headline2>
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
         </View>
       </>
     | Done(Error(error)) =>
       <>
         <ErrorView error />
         <View style=styles##formAction>
           <FormButton text=I18n.btn#ok onPress=onPressCancel />
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
           I18n.title#import_account->React.string
         </Typography.Headline2>
         {switch (formStep) {
          | Step1 =>
            <>
              <Typography.Overline3
                colorStyle=`highEmphasis style=styles##stepPager>
                {I18n.t#stepof(1, 2)->React.string}
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                I18n.title#import_account_enter_phrase->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                I18n.expl#import_account_enter_phrase->React.string
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
                {I18n.t#stepof(2, 2)->React.string}
              </Typography.Overline3>
              <Typography.Overline2 style=styles##stepTitle>
                I18n.title#account_create->React.string
              </Typography.Overline2>
              <Typography.Body3
                colorStyle=`mediumEmphasis style=styles##stepBody>
                I18n.expl#account_create_password_not_recorded->React.string
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
