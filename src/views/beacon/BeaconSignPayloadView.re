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
      ~beaconRespond,
      ~closeAction,
    ) => {
  let onSign = (~password) => {
    Js.log("TODO onSign with password " ++ password);

    let signature = "TODO";

    let _response =
      ReBeacon.Message.ResponseInput.SignPayloadResponse({
        id: signPayloadRequest.id,
        signingType: signPayloadRequest.signingType,
        signature,
      });

    ();
    // beaconRespond(response)
    // ->Future.tapOk(_ => closeAction())
    // ->ignore;
  };

  let onAbort = _ => {
    beaconRespond(
      ReBeacon.Message.ResponseInput.Error({
        id: signPayloadRequest.id,
        errorType: `ABORTED_ERROR,
      }),
    )
    ->Future.tapOk(_ => closeAction())
    ->ignore;
  };

  let (form, formFieldsAreValids) = PasswordFormView.usePasswordForm(onSign);

  <ModalTemplate.Form>
    <View>
      <View style=FormStyles.header>
        <Typography.Headline style=styles##title>
          I18n.title#beacon_sign_request->React.string
        </Typography.Headline>
        <Typography.Overline2
          colorStyle=`highEmphasis fontWeightStyle=`bold style=styles##dapp>
          signPayloadRequest.appMetadata.name->React.string
        </Typography.Overline2>
        <Typography.Overline3 colorStyle=`highEmphasis style=styles##dapp>
          I18n.expl#beacon_dapp_sign->React.string
        </Typography.Overline3>
      </View>
      <OperationSummaryView.AccountInfo
        style=styles##accountInfo
        title=I18n.title#sender_account
        address={signPayloadRequest.sourceAddress}
      />
      <Payload signPayloadRequest />
      <PasswordFormView.PasswordField form />
      <View style=styles##formActionSpaceBetween>
        <Buttons.SubmitSecondary text=I18n.btn#reject onPress=onAbort />
        <Buttons.SubmitPrimary
          text=I18n.btn#sign
          onPress={_event => {form.submit()}}
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </View>
  </ModalTemplate.Form>;
};
