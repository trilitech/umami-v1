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
    "button": style(~alignItems=#center, ~justifyContent=#center, ()),
  })
}

@react.component
let make = (
  ~icon: Icons.builder,
  ~size=28.,
  ~iconSizeRatio=4. /. 7.,
  ~isPrimary=false,
  ~disabled=false,
  ~onPress=?,
  ~tooltip=?,
  ~isActive=?,
  ~pressableRef=?,
  ~toast=false,
  ~style as styleFromProp: option<ReactNative.Style.t>=?,
) => {
  let theme = ThemeContext.useTheme()

  let module(ThemedPressableComp: ThemedPressable.T) = isPrimary
    ? module(ThemedPressable.Primary)
    : module(ThemedPressable.Outline)

  let pressableElement = (~pressableRef=?) =>
    <ThemedPressableComp
      ?pressableRef
      ?onPress
      ?isActive
      disabled
      style={
        open Style
        arrayOption([
          Some(styles["button"]),
          Some(style(~width=size->dp, ~height=size->dp, ~borderRadius=size /. 2., ())),
          styleFromProp,
        ])
      }
      accessibilityRole=#button>
      {icon(
        ~style=?None,
        ~size=Js.Math.ceil_float(iconSizeRatio *. size),
        ~color=switch (disabled, isPrimary, toast) {
        | (true, _, _) => theme.colors.iconDisabled
        | (false, true, false) => theme.colors.primaryIconMediumEmphasis
        | (false, false, false) => theme.colors.iconMediumEmphasis
        | (false, true, true) => theme.colors.iconMediumEmphasis
        | (false, false, true) => theme.colors.toastIconMediumEmphasis
        },
      )}
    </ThemedPressableComp>

  <View>
    {switch tooltip {
    | Some((keyPopover, text)) =>
      <Tooltip keyPopover text> {(~pressableRef) => pressableElement(~pressableRef)} </Tooltip>
    | None => pressableElement(~pressableRef?)
    }}
  </View>
}
