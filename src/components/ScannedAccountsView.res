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
    "content": style(~display=#flex, ()),
    "labelContainer": style(
      ~flexDirection=#row,
      ~justifyContent=#spaceBetween,
      ~marginBottom=4.->dp,
      (),
    ),
    "loading": style(~alignItems=#flexEnd, ()),
    "itemContent": style(
      ~padding=16.->dp,
      ~justifyContent=#spaceBetween,
      ~display=#flex,
      ~flexDirection=#row,
      (),
    ),
    "itemBorder": style(~borderTopWidth=1., ()),
    "itemsContent": style(~height=250.->pt, ~borderWidth=1., ~borderRadius=4., ()),
  })
}

module Item = {
  @react.component
  let make = (~id, ~address) => {
    let theme = ThemeContext.useTheme()

    let requestState = React.useState(() => ApiRequest.NotAsked)

    let balances = BalanceApiRequest.useLoadBalances(~requestState, list{address})
    let amount =
      balances
      ->BalanceApiRequest.getOne(address)
      ->ApiRequest.mapOrLoad(amount =>
        <Typography.Body1
          fontWeightStyle=#bold
          style={
            open Style
            style(~textAlign=#right, ())
          }>
          {I18n.tez_amount(amount->Tez.toString)->React.string}
        </Typography.Body1>
      )

    <AccountElements.Slim
      id
      address
      showAmount=AccountElements.Amount(amount)
      style={
        open Style
        arrayOption([
          {
            open Style
            style(~borderColor=theme.colors.borderDisabled, ())
          }->Some,
          Some(styles["itemContent"]),
          id == 0 ? None : Some(styles["itemBorder"]),
        ])
      }
    />
  }
}

@react.component
let make = (
  ~derivationChangedState as (derivationChanged, setDerivationChanged),
  ~scan,
  ~startIndex=0,
  ~accounts,
  ~nextAdvancedOptions,
  ~next,
  ~path,
  ~scheme,
) => {
  let theme = ThemeContext.useTheme()

  let (scanState, setScanState) = React.useState(() => #Done)

  let (loading, setLoading) = React.useState(() => false)

  React.useEffect3(() => {
    if derivationChanged {
      setScanState(_ => #Loading)
      scan(path, scheme)->Promise.getOk(_ => setScanState(_ => #Done))
      setDerivationChanged(_ => false)
    }

    None
  }, (path, scheme, derivationChanged))

  <>
    <View style={FormStyles.section["spacing"]}>
      <View style={styles["labelContainer"]}>
        <Typography.Overline2> {I18n.Label.accounts->React.string} </Typography.Overline2>
        {<LoadingView style={styles["loading"]} />->ReactUtils.onlyWhen(scanState == #Loading)}
      </View>
      <View
        style={
          open Style
          array([style(~borderColor=theme.colors.borderMediumEmphasis, ()), styles["itemsContent"]])
        }>
        <ScrollView>
          {
            let accounts = accounts->List.toArray
            accounts
            ->Array.mapWithIndex((id, address) =>
              <Item key={(startIndex + id)->Int.toString} address id={startIndex + id} />
            )
            ->React.array
          }
        </ScrollView>
      </View>
    </View>
    <View style={Style.array([FormStyles.verticalFormAction, FormStyles.section["submitSpacing"]])}>
      <Buttons.SubmitPrimary
        disabled={scanState == #Loading}
        text=I18n.Btn.\"import"
        loading
        onPress={_ => {
          setLoading(_ => true)
          next()
        }}
      />
      {<Buttons.FormSecondary
        style=FormStyles.formSecondary
        text=I18n.Btn.advanced_options
        onPress={_ => nextAdvancedOptions->Option.mapWithDefault((), f => f())}
      />->ReactUtils.onlyWhen(nextAdvancedOptions->Option.isSome)}
    </View>
  </>
}
