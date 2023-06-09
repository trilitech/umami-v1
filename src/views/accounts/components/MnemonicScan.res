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

type step =
  | StepPassword
  | StepAccounts(string)

@react.component
let make = (~closeAction, ~index, ~secret) => {
  let config = ConfigContext.useContent()
  let addLog = LogsContext.useAdd()
  let (status, setStatus) = React.useState(() => StepPassword)
  let (accounts, setAccounts) = React.useState(() => list{})
  let derivationChangedState = React.useState(() => true)

  let (importRequest, \"import") = StoreContext.Secrets.useMnemonicImportKeys()

  let onFoundKey = (~start, i, account) =>
    setAccounts(accounts =>
      if (
        accounts->List.some(acc =>
          account.HDWalletAPI.Accounts.Scan.publicKeyHash ==
            acc.HDWalletAPI.Accounts.Scan.publicKeyHash
        )
      ) {
        accounts
      } else {
        let accounts = i == start ? list{} : accounts

        list{account, ...accounts}
      }
    )

  let submitPassword = (~password) =>
    password
    ->SecureStorage.validatePassword
    ->Promise.mapOk(() => setStatus(_ => StepAccounts(password)))

  let submitAccounts = (~password, ()) =>
    \"import"({
      open SecretApiRequest
      {index: index, accounts: accounts->List.reverse, password: password}
    })
    ->ApiRequest.logOk(addLog(true), Logs.Account, _ => I18n.account_created)
    ->Promise.getOk(_ => closeAction())

  let scan = (~password, path, _) =>
    HDWalletAPI.Accounts.Scan.runStreamSeed(
      ~network=config.network,
      ~startIndex=secret.Secret.addresses->Js.Array.length,
      ~onFoundKey=onFoundKey(~start=secret.Secret.addresses->Js.Array.length),
      ~password,
      {
        open Secret
        {index: index, secret: secret}
      },
      path,
    )

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <Typography.Headline style=FormStyles.header>
      {I18n.Title.scan->React.string}
    </Typography.Headline>
    {switch status {
    | StepPassword =>
      <PasswordFormView loading={importRequest->ApiRequest.isLoading} submitPassword />
    | StepAccounts(password) =>
      <ScannedAccountsView
        scan={scan(~password)}
        derivationChangedState
        path=secret.Secret.derivationPath
        scheme=secret.Secret.derivationScheme
        accounts={accounts
        ->List.reverse
        ->List.map(account => account.HDWalletAPI.Accounts.Scan.publicKeyHash)}
        next={submitAccounts(~password)}
        nextAdvancedOptions=None
        startIndex={secret.Secret.addresses->Array.length}
      />
    }}
  </ModalFormView>
}
