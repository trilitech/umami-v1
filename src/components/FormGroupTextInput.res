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
    "formGroup": style(~marginBottom=0.->dp, ()),
    "header": style(~flexDirection=#row, ~justifyContent=#spaceBetween, ~marginVertical=4.->dp, ()),
    "label": style(~marginVertical=4.->dp, ()),
    "decoration": style(
      ~display=#flex,
      ~alignItems=#center,
      ~position=#absolute,
      ~marginTop=auto,
      ~marginBottom=auto,
      ~top=0.->dp,
      ~bottom=0.->dp,
      ~right=10.->dp,
      (),
    ),
  })
}

@react.component
let make = (
  ~label=?,
  ~value,
  ~handleChange,
  ~error,
  ~keyboardType=?,
  ~onBlur=?,
  ~onFocus=?,
  ~onKeyPress=?,
  ~textContentType=?,
  ~secureTextEntry=?,
  ~placeholder=?,
  ~disabled=?,
  ~multiline=?,
  ~numberOfLines=?,
  ~clearButton=false,
  ~onSubmitEditing=?,
  ~decoration: option<(~style: Style.t) => React.element>=?,
  ~style as styleFromProp: option<ReactNative.Style.t>=?,
  ~fieldStyle=?,
  ~tooltipIcon=?,
  ~rightView=?,
) => {
  let hasError = error->Option.isSome
  <FormGroup
    style={
      open Style
      arrayOption([Some(styles["formGroup"]), styleFromProp])
    }>
    {label->Option.mapWithDefault(React.null, a =>
      <View style={styles["header"]}>
        <FormLabel label=a hasError ?tooltipIcon /> {rightView->ReactUtils.mapOpt(view => view)}
      </View>
    )}
    <View>
      <ThemedTextInput
        value
        onValueChange=handleChange
        hasError
        ?onBlur
        ?onFocus
        ?onKeyPress
        ?textContentType
        ?secureTextEntry
        ?keyboardType
        ?placeholder
        ?disabled
        ?multiline
        ?numberOfLines
        ?onSubmitEditing
        style=?fieldStyle
        onClear=?{clearButton ? Some(() => handleChange("")) : None}
      />
      {decoration->ReactUtils.mapOpt(deco => deco(~style=styles["decoration"]))}
    </View>
    <FormError ?error />
  </FormGroup>
}
