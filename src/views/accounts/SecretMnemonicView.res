/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

module RecoveryPhrase = {
  @react.component
  let make = (~recoveryPhrase: string) =>
    <DocumentContext.ScrollView showsVerticalScrollIndicator=true>
      <MnemonicListView mnemonic={recoveryPhrase->Js.String2.split(" ")} />
    </DocumentContext.ScrollView>
}

@react.component
let make = (~secret: Secret.derived, ~closeAction) => {
  let (recoveryPhraseRequest, getRecoveryPhrase) = StoreContext.Secrets.useGetRecoveryPhrase(
    ~index=secret.index,
  )

  let submitPassword = (~password) => getRecoveryPhrase(password)

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <Typography.Headline style=FormStyles.header>
      {I18n.Title.show_recovery_phrase->React.string}
    </Typography.Headline>
    {switch recoveryPhraseRequest {
    | Done(Ok(recoveryPhrase), _) => <RecoveryPhrase recoveryPhrase />
    | _ => <PasswordFormView loading={recoveryPhraseRequest->ApiRequest.isLoading} submitPassword />
    }}
  </ModalFormView>
}
