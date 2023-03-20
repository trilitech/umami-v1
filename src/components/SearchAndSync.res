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
    "searchSection": {
      style(
        ~maxHeight=44.->dp,
        ~flexBasis=48.->dp,
        ~flex=1.,
        ~flexDirection=#row,
        ~alignItems=#center,
        (),
      )
    },
    "searchAndSync": style(~flexDirection=#row, ()),
  })
}

@react.component
let make = (
  ~style=?,
  ~value,
  ~syncIcon,
  ~placeholder,
  ~onValueChange,
  ~onRefresh,
  ~onStop,
  ~syncState,
) => {
  // This style is used to hide the section below when it is scrolled
  let searchAndSyncMarginStyle = {
    open Style
    let theme = ThemeContext.useTheme()
    style(
      ~paddingBottom=16.->dp,
      ~backgroundColor=theme.colors.background,
      ~zIndex=1,
      ()
    )
  }
  <View style={Style.arrayOption([styles["searchAndSync"]->Some, searchAndSyncMarginStyle->Some, style])}>
    <ThemedTextInput
      style={styles["searchSection"]} icon=Icons.Search.build value onValueChange placeholder
    />
    <Sync onRefresh onStop state=syncState icon=syncIcon />
  </View>
}