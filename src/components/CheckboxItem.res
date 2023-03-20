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

module Checkbox = {
  @react.component
  let make = (~value: bool=false) => {
    let theme = ThemeContext.useTheme()
    let size = 20.
    let color = value ? theme.colors.iconPrimary : theme.colors.iconMediumEmphasis
    value ? <Icons.CheckboxSelected size color /> : <Icons.CheckboxUnselected size color />
  }
}

let styles = {
  open Style
  StyleSheet.create({
    "pressable": style(~flexDirection=#row, ~alignItems=#center, ()),
    "label": style(~marginRight=11.->dp, ()),
    "checkboxContainer": style(
      ~marginRight=5.->dp,
      ~height=30.->dp,
      ~width=30.->dp,
      ~borderRadius=15.,
      ~alignItems=#center,
      ~justifyContent=#center,
      (),
    ),
  })
}

@react.component
let make = (
  ~label=?,
  ~value,
  ~handleChange,
  ~hasError=false,
  ~disabled=false,
  ~labelFontWeightStyle=?,
  ~style as styleFromProp: option<Style.t>=?,
  ~labelStyle=?,
) =>
  <Pressable
    style={_ => Style.arrayOption([Some(styles["pressable"]), styleFromProp])}
    onPress={_ => handleChange(!value)}
    disabled
    accessibilityRole=#checkbox>
    {({hovered, pressed, focused}) => {
      let hovered = hovered->Option.getWithDefault(false)
      let focused = focused->Option.getWithDefault(false)
      <>
        <ThemedPressable.ContainerInteractionState.Outline
          hovered pressed focused disabled style={styles["checkboxContainer"]}>
          <Checkbox value />
        </ThemedPressable.ContainerInteractionState.Outline>
        {label->ReactUtils.mapOpt(label =>
          <FormLabel
            label
            hasError
            fontWeightStyle=?labelFontWeightStyle
            style={
              open Style
              arrayOption([Some(styles["label"]), labelStyle])
            }
          />
        )}
      </>
    }}
  </Pressable>
