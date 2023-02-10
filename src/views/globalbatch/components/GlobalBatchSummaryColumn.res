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
open Style
module Label = {
  @react.component
  let make = (~amount) => {
    let theme = ThemeContext.useTheme()

    let style = {
      open Style
      style(
        ~backgroundColor=theme.colors.stateDisabled,
        ~padding="0.8rem"->StyleUtils.stringToSize,
        ~marginTop=4.->dp,
        ~marginBottom=4.->dp,
        ~textAlign=#left,
        ~borderRadius=4.,
        (),
      )
    }

    <View style> <Typography.Subtitle1> {React.string(amount)} </Typography.Subtitle1> </View>
  }
}

module Content = {
  @react.component
  let make = (~content) =>
    <View style={Style.style(~marginTop=10.->dp, ())}>
      {content
      ->List.toArray
      ->Array.map(((property, values)) =>
        <View key=property>
          <Typography.Overline2> {property->React.string} </Typography.Overline2>
          <View>
            {values
            ->List.mapWithIndex((i, value) =>
              <Label key={property ++ (Int.toString(i) ++ value)} amount=value />
            )
            ->List.toArray
            ->React.array}
          </View>
        </View>
      )
      ->React.array}
    </View>
}

@react.component
let make = (~content) => {
  let content: list<(string, Belt.List.t<string>)> =
    content->List.map(((field, amounts)) => (field, amounts->List.map(Protocol.Amount.show)))

  <View> {content->ReactUtils.hideNil(content => <Content content />)} </View>
}
