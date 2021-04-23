module RecoveryPhrase = {
  [@react.component]
  let make = (~recoveryPhrase: string) => {
    <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
      <MnemonicListView mnemonic={recoveryPhrase->Js.String2.split(" ")} />
    </DocumentContext.ScrollView>;
  };
};

[@react.component]
let make = (~secret: Secret.t, ~closeAction) => {
  let (recoveryPhraseRequest, getRecoveryPhrase) =
    StoreContext.Secrets.useGetRecoveryPhrase(~index=secret.index);

  let submitPassword = (~password) => {
    getRecoveryPhrase(password)->ignore;
  };

  <ModalFormView closing={ModalFormView.Close(closeAction)}>
    <Typography.Headline style=FormStyles.header>
      I18n.title#export->React.string
    </Typography.Headline>
    {switch (recoveryPhraseRequest) {
     | Done(Ok(recoveryPhrase), _) => <RecoveryPhrase recoveryPhrase />
     | _ =>
       <PasswordFormView
         loading={recoveryPhraseRequest->ApiRequest.isLoading}
         submitPassword
       />
     }}
  </ModalFormView>;
};
