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
      "qrContainer":
        style(
          ~marginTop=10.->dp,
          ~marginBottom=30.->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "qr": style(~backgroundColor="white", ~padding=10.->dp, ()),
    })
  );

type export = {
  version: Version.t,
  derivationPath: DerivationPath.Pattern.t,
  recoveryPhrase: SecureStorage.Cipher.encryptedData,
};

let encoder = export =>
  Json.Encode.(
    object_([
      ("version", string(export.version->Version.toString)),
      (
        "derivationPath",
        string(export.derivationPath->DerivationPath.Pattern.toString),
      ),
      (
        "recoveryPhrase",
        SecureStorage.Cipher.encoder(export.recoveryPhrase),
      ),
    ])
  );

[@react.component]
let make = (~secret: Secret.derived, ~closeAction) => {
  let (encryptedRecoveryPhraseRequest, getEncryptedRecoveryPhrase) =
    StoreContext.Secrets.useGetEncryptedRecoveryPhrase(~index=secret.index);

  React.useEffect0(() => {
    getEncryptedRecoveryPhrase()->Promise.ignore;
    None;
  });

  <ModalFormView
    title=I18n.Title.export_to_mobile
    titleStyle=FormStyles.headerMarginBottom8
    closing={ModalFormView.Close(closeAction)}>
    <View style=FormStyles.header>
      <Typography.Overline1>
        I18n.Title.scan_with_mobile_to_import->React.string
      </Typography.Overline1>
    </View>
    {switch (encryptedRecoveryPhraseRequest) {
     | Done(Ok(encryptedRecoveryPhrase), _) =>
       <View style=styles##qrContainer>
         <View style=styles##qr>
           <QRCode
             value={
               encoder({
                 version: BackupFile.currentVersion,
                 derivationPath: secret.secret.derivationPath,
                 recoveryPhrase: encryptedRecoveryPhrase,
               })
               ->Js.Json.stringify
             }
             size=200.
           />
         </View>
       </View>
     | _ => React.null
     }}
  </ModalFormView>;
};
