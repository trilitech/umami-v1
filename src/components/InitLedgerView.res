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

let onbStyles = FormStyles.onboarding

type ledgerState = [#Found | #Confirmed | #Denied(Errors.t) | #Loading]

let styles = {
  open Style
  StyleSheet.create({
    "retry": style(~width=100.->pct, ~marginTop=20.->pt, ()),
    "content": style(~textAlign=#center, ~display=#flex, ~alignItems=#center, ()),
    "expl": style(~height=60.->dp, ()),
    "loading": style(~height=50.->dp, ()),
  })
}

let computeTitle = x =>
  switch x {
  | #Found => I18n.Title.hardware_wallet_confirm
  | #Loading => I18n.Title.hardware_wallet_search
  | #Confirmed => I18n.Title.hardware_wallet_confirmed
  | #Denied(ReTaquitoError.LedgerInitTimeout) => I18n.Title.hardware_wallet_not_found
  | #Denied(ReTaquitoError.LedgerKeyRetrieval) => I18n.Title.hardware_wallet_error_app
  | #Denied(ReTaquitoError.LedgerDenied) => I18n.Title.hardware_wallet_denied
  | #Denied(ReTaquitoError.LedgerNotReady) => I18n.Title.hardware_wallet_not_ready
  | _ => I18n.Title.hardware_wallet_error_unknown
  }

let titleComponent = (t, inline, style) =>
  inline
    ? <Typography.Subtitle2 style> t </Typography.Subtitle2>
    : <Typography.Headline style> t </Typography.Headline>

let expl = txt =>
  <Typography.Headline
    fontSize=16.
    style={
      open Style
      array([styles["expl"], FormStyles.section["spacing"]])
    }>
    txt
  </Typography.Headline>

@react.component
let make = (~style=?, ~status, ~retry, ~inline=false) => {
  let theme = ThemeContext.useTheme()

  <View style={Style.arrayOption([style])}>
    {status->computeTitle->React.string->titleComponent(inline, onbStyles["title"])}
    {switch status {
    | #Denied(err) =>
      <View style={styles["content"]}>
        <Icons.CloseOutline
          color=theme.colors.error size=50. style={FormStyles.section["spacing"]}
        />
        {err->Errors.toString->React.string->expl}
        <Buttons.SubmitPrimary text=I18n.Btn.retry style={styles["retry"]} onPress={_ => retry()} />
      </View>
    | #Confirmed =>
      <View style={styles["content"]}>
        <Icons.CheckOutline
          color=theme.colors.valid size=50. style={FormStyles.section["spacing"]}
        />
        {I18n.Expl.hardware_wallet_confirmed->React.string->expl}
      </View>
    | (#Loading | #Found) as st =>
      <View style={styles["content"]}>
        <View
          style={
            open Style
            array([FormStyles.section["spacing"], styles["loading"]])
          }>
          <LoadingView size=ActivityIndicator_Size.large />
        </View>
        {switch st {
        | #Found => I18n.Expl.hardware_wallet_confirm->React.string
        | #Loading => I18n.Expl.hardware_wallet_search->React.string
        }->expl}
      </View>
    }}
  </View>
}
