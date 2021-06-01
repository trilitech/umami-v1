/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "dapp": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "formActionSpaceBetween":
        StyleSheet.flatten([|
          FormStyles.formActionSpaceBetween,
          style(~marginTop=12.->dp, ()),
        |]),
    })
  );

[@react.component]
let make =
    (
      ~permissionRequest: ReBeacon.Message.Request.permissionRequest,
      ~beaconRespond,
      ~closeAction,
    ) => {
  let getAccountPublicKey = AccountApiRequest.useGetPublicKey();
  let initAccount = StoreContext.SelectedAccount.useGet();

  let form =
    BeaconAccountForm.use(
      ~schema={
        BeaconAccountForm.Validation.(
          Schema(
            custom(values => values.account->FormUtils.notNone, Account),
          )
        );
      },
      ~onSubmit=
        ({state}) => {
          switch (state.values.account) {
          | Some(account) =>
            getAccountPublicKey(account)
            ->FutureEx.getOk(publicKey => {
                let response: ReBeacon.Message.ResponseInput.permissionResponse = {
                  id: permissionRequest.id,
                  network: permissionRequest.network,
                  scopes: permissionRequest.scopes,
                  publicKey,
                };

                beaconRespond(
                  response->ReBeacon.Message.ResponseInput.PermissionResponse,
                )
                ->Future.tapOk(_ => {closeAction()})
                ->ignore;
              })
          | None => ()
          };

          None;
        },
      ~initialState={account: initAccount},
      ~i18n=FormUtils.i18n,
      (),
    );

  <ModalTemplate.Form>
    <View>
      <View style=FormStyles.header>
        <Typography.Headline style=styles##title>
          "Connection Request"->React.string
        </Typography.Headline>
        <Typography.Overline2
          colorStyle=`highEmphasis fontWeightStyle=`bold style=styles##dapp>
          permissionRequest.appMetadata.name->React.string
        </Typography.Overline2>
        <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
          "would like to connect to your wallet"->React.string
        </Typography.Overline3>
        <Typography.Body2 colorStyle=`mediumEmphasis style=styles##dapp>
          {js|This site is requesting access to view your account’s address.\nAlways make sure you trust the sites you interact with.|js}
          ->React.string
        </Typography.Body2>
      </View>
      <FormGroupAccountSelector
        label="Account to connect to dApp"
        value={form.values.account}
        handleChange={form.handleChange(Account)}
        error={form.getFieldError(Field(Account))}
      />
      <View style=styles##formActionSpaceBetween>
        <Buttons.SubmitSecondary text="DENY" onPress={_ => closeAction()} />
        <Buttons.SubmitPrimary text="ALLOW" onPress={_ => form.submit()} />
      </View>
    </View>
  </ModalTemplate.Form>;
};
