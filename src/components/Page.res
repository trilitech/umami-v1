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
    "scroll": style(~flex=1., ()),
    "scrollContent": style(
      ~flex=1.,
      ~paddingVertical=LayoutConst.pagePaddingVertical->dp,
      ~paddingHorizontal=LayoutConst.pagePaddingHorizontal->dp,
      (),
    ),
  })
}

@react.component
let make = (~children) =>
  <DocumentContext.ScrollView
    style={styles["scroll"]} contentContainerStyle={styles["scrollContent"]}>
    children
  </DocumentContext.ScrollView>

module Header = {
  let styles = {
    open Style
    StyleSheet.create({
      "header": style(~flexDirection=#row, ~marginBottom=10.->dp, ()),
      "headerContent": style(~flexShrink=1., ~width=100.->pct, ()),
      "actionLeft": style(
        ~marginLeft=40.->dp,
        ~alignItems=#center,
        ~flexGrow=1.,
        ~justifyContent=#flexStart,
        (),
      ),
      "actionRight": style(
        ~marginLeft=auto,
        ~marginRight=24.->dp,
        ~alignItems=#flexEnd,
        ~justifyContent=#flexStart,
        (),
      ),
    })
  }

  @react.component
  let make = (~children, ~left=?, ~right=?, ()) =>
    <View style={styles["header"]}>
      <View style={styles["headerContent"]}>children</View>
      {left->Option.mapWithDefault(React.null, left =>
        <View style={styles["actionLeft"]}> left </View>
      )}
      {right->Option.mapWithDefault(React.null, right =>
        <View style={styles["actionRight"]}> right </View>
      )}
    </View>
}
