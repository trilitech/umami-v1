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

type colorStyle = [
  | #highEmphasis
  | #mediumEmphasis
  | #disabled
  | #error
  | #positive
  | #negative
  | #filledBackHighEmphasis
  | #filledBackMediumEmphasis
  | #filledBackDisabled
  | #primary
]

let getColor = (colorStyle, theme: ThemeContext.theme) =>
  switch colorStyle {
  | #highEmphasis => theme.colors.textHighEmphasis
  | #mediumEmphasis => theme.colors.textMediumEmphasis
  | #disabled => theme.colors.textDisabled
  | #error => theme.colors.error
  | #positive => theme.colors.textPositive
  | #negative => theme.colors.textNegative
  | #filledBackHighEmphasis => theme.colors.primaryTextHighEmphasis
  | #filledBackMediumEmphasis => theme.colors.primaryTextMediumEmphasis
  | #filledBackDisabled => theme.colors.primaryTextDisabled
  | #primary => theme.colors.textPrimary
  }

type fontWeightStyle = [
  | #black
  | #extraBold
  | #bold
  | #semiBold
  | #medium
  | #regular
  | #light
  | #extraLight
]

let getFontWeight = fontWeightStyle =>
  switch fontWeightStyle {
  | #black => #_900
  | #extraBold => #_800
  | #bold => #bold
  | #semiBold => #_600
  | #medium => #_500
  | #regular => #normal
  | #light => #_300
  | #extraLight => #_200
  }

module type TextDesignStyle = {
  let colorStyle: colorStyle
  let fontWeightStyle: fontWeightStyle
  let fontSize: float
  let selectable: bool
}

module Base = {
  let styles = {
    open Style
    StyleSheet.create({"text": style(~fontFamily="SourceSansPro", ())})
  }

  @react.component
  let make = (
    ~colorStyle: colorStyle,
    ~fontSize: float,
    ~fontWeightStyle: fontWeightStyle,
    ~numberOfLines: option<int>=?,
    ~style as styleProp: option<ReactNative.Style.t>=?,
    ~ellipsizeMode: option<[#clip | #head | #middle | #tail]>=?,
    ~selectable: option<bool>=?,
    ~onPress=?,
    ~children: React.element,
  ) => {
    let theme = ThemeContext.useTheme()
    <Text
      ?ellipsizeMode
      ?numberOfLines
      ?selectable
      ?onPress
      style={
        open Style
        arrayOption([
          Some(styles["text"]),
          Some(
            style(
              ~color=colorStyle->getColor(theme),
              ~fontSize,
              ~fontWeight=fontWeightStyle->getFontWeight,
              (),
            ),
          ),
          styleProp,
        ])
      }>
      children
    </Text>
  }
}

module Make = (DefaultStyle: TextDesignStyle) => {
  let makeProps = (
    ~colorStyle=DefaultStyle.colorStyle,
    ~fontSize=DefaultStyle.fontSize,
    ~fontWeightStyle=DefaultStyle.fontWeightStyle,
    ~selectable=DefaultStyle.selectable,
    ~numberOfLines=?,
    ~ellipsizeMode=?,
    ~style=?,
    ~children,
  ) =>
    Base.makeProps(
      ~colorStyle,
      ~fontSize,
      ~fontWeightStyle,
      ~selectable,
      ~numberOfLines?,
      ~ellipsizeMode?,
      ~style?,
      ~children,
    )
  let make = Base.make
}

/* H */

module Headline = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #bold
  let fontSize = 22.
  let selectable = true
})

let headline = (~colorStyle=?, ~fontWeightStyle=?, ~fontSize=?, ~selectable=?, ~style=?, txt) =>
  <Headline ?colorStyle ?fontWeightStyle ?fontSize ?selectable ?style>
    {txt->React.string}
  </Headline>

/* OVERLINE */

module Overline1 = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #light
  let fontSize = 19.
  let selectable = true
})

let overline1 = (~colorStyle=?, ~fontWeightStyle=?, ~fontSize=?, ~selectable=?, ~style=?, txt) =>
  <Overline1 ?colorStyle ?fontWeightStyle ?fontSize ?selectable ?style>
    {txt->React.string}
  </Overline1>

module Overline2 = Make({
  let colorStyle = #mediumEmphasis
  let fontWeightStyle = #semiBold
  let fontSize = 16.
  let selectable = true
})

let overline2 = (~colorStyle=?, ~fontWeightStyle=?, ~fontSize=?, ~selectable=?, ~style=?, txt) =>
  <Overline2 ?colorStyle ?fontWeightStyle ?fontSize ?selectable ?style>
    {txt->React.string}
  </Overline2>

module Overline3 = Make({
  let colorStyle = #mediumEmphasis
  let fontWeightStyle = #regular
  let fontSize = 16.
  let selectable = true
})

let overline3 = (~colorStyle=?, ~fontWeightStyle=?, ~fontSize=?, ~selectable=?, ~style=?, txt) =>
  <Overline3 ?colorStyle ?fontWeightStyle ?fontSize ?selectable ?style>
    {txt->React.string}
  </Overline3>

/* SUBTITLE */

module Subtitle1 = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #semiBold
  let fontSize = 16.
  let selectable = true
})

let subtitle1 = (~style=?, txt) => <Subtitle1 ?style> {txt->React.string} </Subtitle1>

module Subtitle2 = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #bold
  let fontSize = 16.
  let selectable = true
})

let subtitle2 = (~style=?, txt) => <Subtitle2 ?style> {txt->React.string} </Subtitle2>

/* BODY */

module Body1 = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #regular
  let fontSize = 16.
  let selectable = true
})
let body1 = (~numberOfLines=?, ~style=?, txt) =>
  <Body1 ?numberOfLines ?style> {txt->React.string} </Body1>

module Body2 = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #regular
  let fontSize = 14.
  let selectable = true
})
let body2 = txt => <Body2> {txt->React.string} </Body2>

module Contract = Make({
  let colorStyle = #mediumEmphasis
  let fontWeightStyle = #medium
  let fontSize = 14.
  let selectable = true
})
let contract = txt => <Contract> {txt->React.string} </Contract>

/* BUTTON */

module ButtonPrimary = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #bold
  let fontSize = 14.
  let selectable = false
})

module ButtonSecondary = Make({
  let colorStyle = #mediumEmphasis
  let fontWeightStyle = #bold
  let fontSize = 12.
  let selectable = false
})

module ButtonTernary = Make({
  let colorStyle = #mediumEmphasis
  let fontWeightStyle = #semiBold
  let fontSize = 11.
  let selectable = false
})

/* ADDRESS */

module Address = {
  let styles = {
    open Style
    StyleSheet.create({"address": style(~fontFamily="JetBrainsMono", ())})
  }

  @react.component
  let make = (
    ~colorStyle=#highEmphasis,
    ~numberOfLines: option<int>=?,
    ~style as styleProp: option<ReactNative.Style.t>=?,
    ~fontSize=14.,
    ~children,
  ) =>
    <Base
      colorStyle
      fontSize
      fontWeightStyle=#extraLight
      style={
        open Style
        arrayOption([Some(styles["address"]), styleProp])
      }
      ?numberOfLines>
      children
    </Base>
}

let address = (~colorStyle=?, ~numberOfLines=?, ~style=?, ~fontSize=?, txt) =>
  <Address ?colorStyle ?numberOfLines ?style ?fontSize> {txt->React.string} </Address>

/* NOTICE */

module Notice = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #bold
  let fontSize = 14.
  let selectable = true
})

/* NFT */

module BigText = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #medium
  let fontSize = 34.
  let selectable = true
})

module MediumText = Make({
  let colorStyle = #highEmphasis
  let fontWeightStyle = #medium
  let fontSize = 24.
  let selectable = true
})

/* LOGS */

module URI = Make({
  let colorStyle = #primary
  let fontWeightStyle = #regular
  let fontSize = 16.
  let selectable = true
})

module InPageLog = {
  let styles = {
    open Style
    StyleSheet.create({"log": style(~fontFamily="JetBrainsMono", ())})
  }
  @react.component
  let make = (
    ~style as styleProp: option<ReactNative.Style.t>=?,
    ~ellipsizeMode: option<[#clip | #head | #middle | #tail]>=?,
    ~fontWeightStyle: fontWeightStyle,
    ~numberOfLines: option<int>=?,
    ~content,
  ) =>
    <Text
      ?ellipsizeMode
      ?numberOfLines
      style={
        open Style
        arrayOption([
          Some(styles["log"]),
          Some(style(~fontWeight=fontWeightStyle->getFontWeight, ~fontSize=14., ())),
          styleProp,
        ])
      }>
      content
    </Text>
}
