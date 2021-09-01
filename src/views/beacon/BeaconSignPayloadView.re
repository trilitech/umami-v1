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

module Payload = {
  let styles =
    Style.(
      StyleSheet.create({
        "container": style(~marginBottom=20.->dp, ()),
        "payloadBloc":
          style(
            ~minHeight=220.->dp,
            ~paddingVertical=12.->dp,
            ~paddingHorizontal=12.->dp,
            ~borderWidth=1.,
            ~borderRadius=4.,
            (),
          ),
        "clipboard":
          style(~position=`absolute, ~right=8.->dp, ~bottom=8.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (~signPayloadRequest: ReBeacon.Message.Request.signPayloadRequest) => {
    let theme = ThemeContext.useTheme();
    let addToast = LogsContext.useToast();
    <>
      <Typography.Overline2
        colorStyle=`mediumEmphasis style=OperationSummaryView.styles##title>
        I18n.label#beacon_sign_payload->React.string
      </Typography.Overline2>
      <View style=styles##container>
        <View
          style=Style.(
            array([|
              styles##payloadBloc,
              style(~borderColor=theme.colors.borderDisabled, ()),
            |])
          )>
          <Typography.Address>
            signPayloadRequest.payload->React.string
          </Typography.Address>
        </View>
        <View style=styles##clipboard>
          <ClipboardButton
            copied=I18n.log#beacon_sign_payload
            tooltipKey="beacon-sign-payload"
            addToast
            data={signPayloadRequest.payload}
            size=40.
          />
        </View>
      </View>
    </>;
  };
};

let styles =
  Style.(
    StyleSheet.create({
      "title": style(~marginBottom=8.->dp, ~textAlign=`center, ()),
      "dapp": style(~marginBottom=4.->dp, ~textAlign=`center, ()),
      "accountInfo": style(~marginBottom=24.->dp, ()),
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
      ~signPayloadRequest: ReBeacon.Message.Request.signPayloadRequest,
      ~closeAction,
    ) => {
  let signPayload = BeaconApiRequest.Signature.useSignPayload();

  let (loading, setLoading) = React.useState(() => false);

  let (client, _) = StoreContext.Beacon.useClient();

  let onSign = (~signingIntent) => {
    setLoading(_ => true);
    signPayload(
      ~source=signPayloadRequest.sourceAddress,
      ~signingIntent,
      ~payload=signPayloadRequest.payload,
    )
    ->Future.tap(
        fun
        | Ok(signature) => {
            setLoading(_ => false);
            client
            ->FutureEx.fromOption(
                ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
              )
            ->Future.flatMapOk(client =>
                client->ReBeacon.WalletClient.respond(
                  `SignPayloadResponse({
                    type_: `sign_payload_response,
                    id: signPayloadRequest.id,
                    signingType: signPayloadRequest.signingType,
                    signature: signature.prefixSig,
                  }),
                )
              )
            ->FutureEx.getOk(_ => closeAction());
          }
        | Error(_) => {
            setLoading(_ => false);
            client
            ->FutureEx.fromOption(
                ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
              )
            ->Future.flatMapOk(client =>
                client->ReBeacon.WalletClient.respond(
                  `Error({
                    type_: `error,
                    id: signPayloadRequest.id,
                    errorType: `SIGNATURE_TYPE_NOT_SUPPORTED,
                  }),
                )
              )
            ->FutureEx.getOk(_ => closeAction());
          },
      );
  };

  let onAbort = _ => {
    setLoading(_ => false);
    client
    ->FutureEx.fromOption(
        ~error=Errors.Generic(I18n.errors#beacon_client_not_created),
      )
    ->Future.flatMapOk(client =>
        client->ReBeacon.WalletClient.respond(
          `Error({
            type_: `error,
            id: signPayloadRequest.id,
            errorType: `ABORTED_ERROR,
          }),
        )
      )
    ->FutureEx.getOk(_ => closeAction());
  };

  let ledgerState = React.useState(() => None);
  let isLedger =
    StoreContext.Accounts.useIsLedger(signPayloadRequest.sourceAddress);

  let sendOperation = intent => onSign(~signingIntent=intent);

  let secondaryButton =
    <Buttons.SubmitSecondary text=I18n.btn#reject onPress=onAbort />;

  <ModalFormView title=I18n.title#beacon_sign_request>
    <View style=FormStyles.header>
      <Typography.Overline2
        colorStyle=`highEmphasis fontWeightStyle=`bold style=styles##dapp>
        signPayloadRequest.appMetadata.name->React.string
      </Typography.Overline2>
      <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
        I18n.expl#beacon_dapp_sign->React.string
      </Typography.Overline3>
    </View>
    <OperationSummaryView.EntityInfo
      style=styles##accountInfo
      title=I18n.title#sender_account
      address={signPayloadRequest.sourceAddress->Some}
    />
    <Payload signPayloadRequest />
    <SigningBlock isLedger ledgerState sendOperation loading secondaryButton />
  </ModalFormView>;
};
