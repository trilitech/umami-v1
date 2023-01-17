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

open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "title": style(~marginBottom=8.->dp, ~textAlign=#center, ()),
    "dapp": style(~marginBottom=4.->dp, ~textAlign=#center, ()),
    "formActionSpaceBetween": StyleSheet.flatten([
      FormStyles.formActionSpaceBetween,
      style(~marginTop=12.->dp, ()),
    ]),
  })
}

@react.component
let make = (
  ~account: Account.t,
  ~permissionRequest: Beacon.Message.Request.permissionRequest,
  ~closeAction,
) => {
  let getAccountPublicKey = AccountApiRequest.useGetPublicKey()

  let (account, setAccount) = React.useState(() => account)

  let updatePermissions = StoreContext.Beacon.Permissions.useResetAll()

  let (client, _) = StoreContext.Beacon.useClient()

  let submit = () =>
    getAccountPublicKey(account)->Promise.getOk(publicKey =>
      client
      ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
      ->Promise.flatMapOk(client =>
        client->ReBeacon.WalletClient.respond(
          #PermissionResponse({
            type_: #permission_response,
            id: permissionRequest.id,
            network: permissionRequest.network,
            scopes: permissionRequest.scopes,
            publicKey: publicKey,
          }),
        )
      )
      ->Promise.getOk(_ => {
        updatePermissions()
        closeAction()
      })
    )

  let onAbort = _ =>
    client
    ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
    ->Promise.flatMapOk(client =>
      client->ReBeacon.WalletClient.respond(
        #Error({
          type_: #error,
          id: permissionRequest.id,
          errorType: #ABORTED_ERROR,
        }),
      )
    )
    ->Promise.getOk(_ => closeAction())

  <ModalTemplate.Form>
    <View>
      <View style=FormStyles.header>
        <Typography.Headline style={styles["title"]}>
          {I18n.Title.beacon_connection_request->React.string}
        </Typography.Headline>
        <Typography.Overline2 colorStyle=#highEmphasis fontWeightStyle=#bold style={styles["dapp"]}>
          {permissionRequest.appMetadata.name->React.string}
        </Typography.Overline2>
        <Typography.Overline3 colorStyle=#highEmphasis style={styles["dapp"]}>
          {I18n.Expl.beacon_dapp->React.string}
        </Typography.Overline3>
        <Typography.Body2 colorStyle=#mediumEmphasis style={styles["dapp"]}>
          {I18n.Expl.beacon_dapp_request->React.string}
        </Typography.Body2>
      </View>
      <FormGroupAccountSelector.Accounts
        label=I18n.Label.beacon_account value=account handleChange={a => setAccount(_ => a)}
      />
      <View style={styles["formActionSpaceBetween"]}>
        <Buttons.SubmitSecondary text=I18n.Btn.deny onPress=onAbort />
        <Buttons.SubmitPrimary text=I18n.Btn.allow onPress={_ => submit()} />
      </View>
    </View>
  </ModalTemplate.Form>
}
