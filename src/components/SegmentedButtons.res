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
    "element": style(
      ~flexDirection=#row,
      ~alignItems=#center,
      ~paddingHorizontal=16.->dp,
      ~paddingVertical=8.->dp,
      (),
    ),
  })
}

@react.component
let make = (~selectedValue, ~setSelectedValue, ~buttons) => {
  let theme = ThemeContext.useTheme()

  <View
    style={
      open Style
      style(
        ~flexDirection=#row,
        ~borderRadius=5.,
        ~marginVertical=8.->dp,
        ~backgroundColor=theme.colors.stateHovered,
        (),
      )
    }>
    {buttons
    ->Array.mapWithIndex((index, button) => {
      let (title, value, dot) = button
      let borderStyle = {
        open Style
        if index == 0 {
          Some(style(~borderTopLeftRadius=5., ~borderBottomLeftRadius=5., ()))
        } else if index == buttons->Array.length - 1 {
          Some(style(~borderTopRightRadius=5., ~borderBottomRightRadius=5., ()))
        } else {
          None
        }
      }
      <ThemedPressable
        key=title
        style={Style.arrayOption([Some(styles["element"]), borderStyle])}
        onPress={_ => setSelectedValue(_ => value)}
        isActive={selectedValue == value}>
        {title->Typography.subtitle1}
        {dot
          ? <Buttons.Dot
              style={Style.style(~position=#relative, ~top="-0.25rem"->StyleUtils.stringToSize, ())}
            />
          : React.null}
      </ThemedPressable>
    })
    ->React.array}
  </View>
}
