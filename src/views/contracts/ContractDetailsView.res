/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2023 Nomadic Labs, <contact@nomadic-labs.com> */
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
    "info": style(
      ~borderRadius=4.,
      ~maxWidth=415.->dp,
      ~minHeight=44.->dp,
      ~marginTop=4.->dp,
      ~marginBottom=8.->dp,
      ~paddingHorizontal=16.->dp,
      ~paddingVertical=12.->dp,
      ()),
    "title": style(~marginBottom=6.->dp, ~textAlign=#center, ()),
    "overline": style(~marginBottom=24.->dp, ~textAlign=#center, ()),
    "tag": style(~marginBottom=6.->dp, ~alignSelf=#center, ()),
    "tagContent": style(~paddingHorizontal=12.->dp, ()),
  })
}

module Info = {
  @react.component
  let make = (~children: React.element) => {
    let theme = ThemeContext.useTheme()
    let backStyle = {open Style;style(~backgroundColor=theme.colors.stateDisabled, (),)}
    <View style={Style.array([styles["info"], backStyle])}> children </View>
  }
}

module Title = {
  @react.component
  let make = (~text: string) => {
    <Typography.Headline style={styles["title"]}>
      {text->React.string}
    </Typography.Headline>
  }
}

module Tag = {
  @react.component
  let make = (~content: string) => {
    <Tag
      fontSize=14.
      height=26.
      style={styles["tag"]}
      contentStyle={styles["tagContent"]}
      content
    />
  }
}

module Overline = {
  @react.component
  let make = (~text: string) => {
    <Typography.Overline3 style={styles["overline"]}>
      {text->React.string}
    </Typography.Overline3>
  }
}

module Multisig = {

  module Name = {
    @react.component
    let make = (~name) => {
      <>
        <Typography.Subtitle1> {I18n.Title.contract_name->React.string} </Typography.Subtitle1>
        <Info> <Typography.Body1> {name->React.string} </Typography.Body1> </Info>
      </>
    }
  }

  module Owners = {
    @react.component
    let make = (~owners) => {
      <>
        <Typography.Subtitle1 style={open Style;style(~marginTop=16.->dp, ())}>
          {I18n.Title.owners(owners->Array.length->Int.toString)->React.string}
        </Typography.Subtitle1>
        {
          owners
          ->Array.mapWithIndex((i, owner) =>
            <OperationSummaryView.EntityInfo
              key={i->Int.toString}
              style={open Style;style(~maxWidth=415.->dp, ~marginVertical=4.->dp, ())}
              address={Some(owner->FormUtils.Alias.address)}
            />)
          ->React.array
        }
      </>
    }
  }

  module Threshold = {
    @react.component
    let make = (~threshold, ~owners) => {
      <>
        <Typography.Subtitle1 style={open Style;style(~marginTop=16.->dp, ())}>
          {I18n.Title.approval_threshold->React.string}
        </Typography.Subtitle1>
        <Info>
          <Typography.Body1>
            {I18n.Label.out_of(threshold->Int.toString, owners->Int.toString)->React.string}
          </Typography.Body1>
        </Info>
      </>
    }
  }

}