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
    "wordsList": style(~flexDirection=#row, ~flexWrap=#wrap, ()),
    "wordItem": style(
      ~marginVertical=2.->dp,
      ~flexGrow=1.,
      ~flexShrink=1.,
      ~flexBasis=40.->pct,
      ~paddingHorizontal=10.->dp,
      ~flexDirection=#row,
      ~alignItems=#center,
      ~height=36.->dp,
      ~borderWidth=1.,
      ~borderRadius=4.,
      ~borderStyle=#dashed,
      (),
    ),
    "wordItemInner": style(~flexDirection=#row, ~alignItems=#baseline, ()),
    "wordItemIndex": style(~width=17.->dp, ~marginRight=13.->dp, ~textAlign=#right, ()),
    "wordSpacer": style(~width=20.->dp, ()),
  })
}

@react.component
let make = (~mnemonic) => {
  let theme = ThemeContext.useTheme()

  <View style={styles["wordsList"]}>
    {mnemonic
    ->Array.mapWithIndex((index, word) =>
      <React.Fragment key={word ++ index->string_of_int}>
        <View
          style={
            open Style
            array([styles["wordItem"], style(~borderColor=theme.colors.borderDisabled, ())])
          }>
          <View style={styles["wordItemInner"]}>
            <Typography.Subtitle1 colorStyle=#disabled style={styles["wordItemIndex"]}>
              {(index + 1)->string_of_int->React.string}
            </Typography.Subtitle1>
            <Typography.Body1> {word->React.string} </Typography.Body1>
          </View>
        </View>
        {mod(index, 2) == 0 ? <View style={styles["wordSpacer"]} /> : React.null}
      </React.Fragment>
    )
    ->React.array}
  </View>
}
