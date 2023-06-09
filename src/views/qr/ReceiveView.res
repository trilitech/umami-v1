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
    "qrContainer": style(
      ~marginTop=10.->dp,
      ~marginBottom=30.->dp,
      ~alignItems=#center,
      ~justifyContent=#center,
      (),
    ),
    "qr": style(~backgroundColor="white", ~padding=10.->dp, ()),
    "addressContainer": style(
      ~flexDirection=#row,
      ~justifyContent=#center,
      ~alignItems=#center,
      (),
    ),
    "address": style(~marginRight=4.->dp, ()),
    "closeAction": style(~position=#absolute, ~right=20.->dp, ~top=20.->dp, ()),
  })
}

module GenericReceiveView = {
  @react.component
  let make = (~name: string, ~address: string, ~onPressCancel) => {
    let addToast = LogsContext.useToast()

    <ModalTemplate.Form headerRight={<ModalTemplate.HeaderButtons.Close onPress=onPressCancel />}>
      <Typography.Headline style=FormStyles.header>{name->React.string}</Typography.Headline>
    <View style={styles["qrContainer"]}>
      <View style={styles["qr"]}> <QRCode value=address size=200. /> </View>
    </View>
    <View style={styles["addressContainer"]}>
      <Typography.Address style={styles["address"]}>{address->React.string}</Typography.Address>
      <ClipboardButton copied=I18n.Log.address tooltipKey="QrView" addToast data=address />
    </View>
  </ModalTemplate.Form>
  }
}

@react.component
let make = (~account: Alias.t, ~onPressCancel) => {
  <GenericReceiveView name={account.name} address={(account.address :> string)} onPressCancel />
}
