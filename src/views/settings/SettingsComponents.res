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

module SettingTextInput = {
  let styles = {
    open Style
    StyleSheet.create({"input": style(~height=36.->dp, ())})
  }

  @react.component
  let make = (
    ~value,
    ~onValueChange,
    ~error,
    ~keyboardType=?,
    ~onSubmitEditing=?,
    ~placeholder=?,
    ~style as styleFromProp=?,
  ) => <>
    <View>
      <ThemedTextInput
        style={
          open Style
          arrayOption([Some(styles["input"]), styleFromProp])
        }
        paddingLeft=16.
        paddingVertical=8.
        value
        onValueChange
        hasError={error->Belt.Option.isSome}
        ?keyboardType
        ?placeholder
        ?onSubmitEditing
        onClear={() => onValueChange("")}
      />
    </View>
    <FormError ?error />
  </>
}

module SettingFormGroupTextInput = {
  let styles = {
    open Style
    StyleSheet.create({
      "header": style(~flexDirection=#row, ~justifyContent=#spaceBetween, ~marginBottom=6.->dp, ()),
      "label": style(~marginBottom=0.->dp, ()),
      "formGroup": style(~flex=1., ~marginTop=0.->dp, ~marginBottom=0.->dp, ()),
    })
  }

  @react.component
  let make = (
    ~label,
    ~value,
    ~onValueChange,
    ~error,
    ~keyboardType=?,
    ~onSubmitEditing=?,
    ~placeholder=?,
    ~rightView=?,
  ) => {
    let hasError = error->Option.isSome
    <FormGroup style={styles["formGroup"]}>
      <View style={styles["header"]}>
        <FormLabel label hasError style={styles["label"]} />
        {rightView->ReactUtils.mapOpt(view => view)}
      </View>
      <SettingTextInput value onValueChange error ?keyboardType ?onSubmitEditing ?placeholder />
    </FormGroup>
  }
}

module ColumnLeft = {
  let styles = {
    open Style
    StyleSheet.create({
      "column": style(~flexGrow=3., ~flexShrink=3., ~flexBasis=0.->dp, ()),
    })
  }

  @react.component
  let make = (~style as styleFromProp=?, ~children) =>
    <View
      style={
        open Style
        arrayOption([Some(styles["column"]), styleFromProp])
      }>
      children
    </View>
}

module ColumnRight = {
  let styles = {
    open Style
    StyleSheet.create({
      "column": style(~flexGrow=4., ~flexShrink=4., ~flexBasis=0.->dp, ~marginLeft=16.->dp, ()),
    })
  }

  @react.component
  let make = (~style as styleFromProp=?, ~children=?) =>
    <View
      style={
        open Style
        arrayOption([Some(styles["column"]), styleFromProp])
      }>
      {children->Belt.Option.getWithDefault(React.null)}
    </View>
}

module Block = {
  let styles = {
    open Style
    StyleSheet.create({
      "bloc": style(~paddingBottom=32.->dp, ~marginBottom=16.->dp, ()),
      "title": style(~marginLeft=16.->dp, ~marginBottom=18.->dp, ()),
      "titleaction": style(~flexDirection=#row, ~justifyContent=#spaceBetween, ()),
      "content": style(~flexDirection=#row, ~paddingHorizontal=30.->dp, ()),
    })
  }

  @react.component
  let make = (~title=?, ~children, ~isLast=false, ~actionButton=?) => {
    let theme = ThemeContext.useTheme()

    <View
      style={
        open Style
        array([
          styles["bloc"],
          style(
            ~borderBottomColor=theme.colors.borderDisabled,
            ~borderBottomWidth=isLast ? 0. : 1.,
            (),
          ),
        ])
      }>
      {title->Option.mapWithDefault(React.null, title =>
        <View style={styles["titleaction"]}>
          {title->Typography.headline(~fontSize=16., ~style={styles["title"]})}
          {actionButton->ReactUtils.opt}
        </View>
      )}
      <View style={styles["content"]}> children </View>
    </View>
  }
}
