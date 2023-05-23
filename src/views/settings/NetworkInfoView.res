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

module StateLenses = %lenses(
  type state = {
    node: string,
  }
)
module NetworkInfoCreateForm = ReForm.Make(StateLenses)

let styles = {
  open Style
  StyleSheet.create({
    "buttonsRow": style(
      ~flexDirection=#row,
      ~alignItems=#center,
      ~justifyContent=#spaceBetween,
      (),
    ),
    "container": style(~flexDirection=#column, ()),
    "notFirstItem": style(~marginTop=24.->dp, ()),
    "content": style(
      ~borderRadius=4.,
      ~paddingVertical=2.->dp,
      ~paddingLeft=16.->dp,
      ~paddingRight=2.->dp,
      (),
    ),
    "innerItem": style(~justifyContent=#spaceBetween, ~flexDirection=#row, ~alignItems=#center, ()),
    "title": style(~marginBottom=6.->dp, ~textAlign=#center, ()),
    "overline": style(~marginBottom=24.->dp, ~textAlign=#center, ()),
  })
}

module Item = {
  @react.component
  let make = (~style as styleArg=?, ~current=?, ~values, ~label) => {
    let theme = ThemeContext.useTheme()
    let addToast = LogsContext.useToast()

    let backStyle = {
      open Style
      style(
        ~color=theme.colors.textMediumEmphasis,
        ~backgroundColor=theme.colors.stateDisabled,
        ~borderWidth=0.,
        (),
      )
    }

    <View style={Style.arrayOption([styleArg, styles["container"]->Some])}>
      <FormLabel label style={FormGroupTextInput.styles["label"]} />
      <View style={Style.array([styles["content"], backStyle])}>
        {values
        ->Array.map(v => {
          let fontWeightStyle = current == Some(v) ? Some(#bold) : None

          <View key=v style={styles["innerItem"]}>
            <Typography.Body1 ?fontWeightStyle> {v->React.string} </Typography.Body1>
            <ClipboardButton size=40. copied=label addToast data=v />
          </View>
        })
        ->React.array}
      </View>
    </View>
  }
}

@react.component
let make = (~network: Network.network, ~closeAction) => {
  let currentEndpoint = ConfigContext.useContent().network.endpoint

  let endpoints = switch network.chain {
  | #...Network.nativeChains as n => Network.getNetworks(n)->List.map(n => n.endpoint)->List.toArray
  | _ => [network.endpoint]
  }

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <Typography.Headline style=FormStyles.headerWithoutMarginBottom>
      {network.name->React.string}
    </Typography.Headline>
    <Item
      current=currentEndpoint
      label={I18n.Label.custom_network_node_url(endpoints->Array.length > 0)}
      values=endpoints
    />
  </ModalFormView>
}
