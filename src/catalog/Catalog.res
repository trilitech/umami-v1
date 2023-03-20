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
open ReactNative.Style
module ThemeContext = {
  let useThemeState = () => {
    let (theme, set) = React.useState(() => #light)
    let setTheme = t => set(_ => t)
    (theme, setTheme)
  }

  @react.component
  let make = (~children) => {
    let (theme, setTheme) = useThemeState()

    <ThemeContextBase theme setTheme> children </ThemeContextBase>
  }
}

module Wrapper = {
  @react.component
  let make = (~children) =>
    <View
      style={style(
        ~width=800.->dp,
        ~minHeight="100vh"->StyleUtils.stringToSize,
        ~margin="auto"->StyleUtils.stringToSize,
        (),
      )}>
      children
    </View>
}

module Menu = {
  @react.component
  let make = () =>
    <View
      style={style(~top=20.->dp, ~left=20.->dp, ~width=200.->dp, ())->unsafeAddStyle({
        "position": "fixed",
      })}>
      <ThemeSettingView />
      <Typography.Overline1 onPress={_ => RescriptReactRouter.push("/typography")}>
        {"typography"->React.string}
      </Typography.Overline1>
      <Typography.Overline1 onPress={_ => RescriptReactRouter.push("/buttons")}>
        {"buttons"->React.string}
      </Typography.Overline1>
      <Typography.Overline1 onPress={_ => RescriptReactRouter.push("/mnemonic")}>
        {"mnemonic"->React.string}
      </Typography.Overline1>
    </View>
}

@react.component
let make = () => {
  let url = RescriptReactRouter.useUrl()

  <ThemeContext>
    <Background>
      <Menu />
      <Wrapper>
        {switch url.path {
        | list{"typography"} => <CatalogText />
        | list{"buttons"} => <CatalogButtons />
        | list{"mnemonic"} => <CatalogMneumonic />
        | _ => <CatalogButtons />
        }}
      </Wrapper>
    </Background>
  </ThemeContext>
}
