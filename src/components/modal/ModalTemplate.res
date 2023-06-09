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

module HeaderButtons = {
  module Close = {
    @react.component
    let make = (~disabled=?, ~tooltip=?, ~onPress) =>
      <IconButton
        icon=Icons.Close.build ?disabled ?tooltip onPress size=48. iconSizeRatio={3. /. 4.}
      />
  }

  module Back = {
    @react.component
    let make = (~onPress) =>
      <IconButton icon=Icons.ArrowLeft.build onPress size=48. iconSizeRatio={3. /. 4.} />
  }
}

let styles = {
  open Style
  StyleSheet.create({
    "headerLeft": style(~position=#absolute, ~left=14.->dp, ~top=14.->dp, ()),
    "headerRight": style(~position=#absolute, ~right=14.->dp, ~top=14.->dp, ()),
    "loadingView": StyleSheet.flatten([
      StyleSheet.absoluteFillObject,
      style(~justifyContent=#center, ~alignItems=#center, ()),
    ]),
    "modal": style(~alignSelf=#center, ~borderRadius=4., ())->unsafeAddStyle({
      "boxShadow": "0 7px 8px -4px rgba(0, 0, 0, 0.2), 0 5px 22px 4px rgba(0, 0, 0, 0.12), 0 12px 17px 2px rgba(0, 0, 0, 0.14)",
    }),
    "modalForm": style(
      ~width=575.->dp,
      ~paddingTop=35.->dp,
      ~paddingBottom=32.->dp,
      ~paddingRight=77.->dp,
      ~paddingLeft=77.->dp,
      (),
    ),
    "scrollview": style(~paddingBottom=3.->dp, ~paddingHorizontal=3.->dp, ()),
    "fullScreenWidth": style(~width=100.->pct, ()),
    "fullScreenHeight": style(~height=100.->pct, ()),
    "modalDialog": style(
      ~width=522.->dp,
      ~paddingTop=40.->dp,
      ~paddingBottom=40.->dp,
      ~paddingHorizontal=50.->dp,
      (),
    ),
  })
}

module Base = {
  @react.component
  let make = (
    ~children,
    ~headerLeft=?,
    ~headerRight=?,
    ~headerCenter=?,
    ~loading as (loadingState, loadingTitle)=(false, None),
    ~style as styleFromProp=?,
    ~contentContainerStyle=?,
  ) => {
    let theme = ThemeContext.useTheme()

    <View
      style={
        open Style
        arrayOption([
          Some(styles["modal"]),
          Some(style(~backgroundColor=theme.colors.background, ())),
          styleFromProp,
        ])
      }>
      {headerCenter->ReactUtils.opt}
      <DocumentContext.ScrollView
        style={styles["scrollview"]} ?contentContainerStyle showsVerticalScrollIndicator=true>
        children
        {loadingState
          ? <View
              style={
                open Style
                array([
                  styles["loadingView"],
                  style(~backgroundColor=theme.colors.background, ~opacity=0.87, ()),
                ])
              }>
              {loadingTitle->ReactUtils.mapOpt(loadingTitle =>
                <Typography.Headline style=FormStyles.header>
                  {loadingTitle->React.string}
                </Typography.Headline>
              )}
              <ActivityIndicator
                animating=true
                size=ActivityIndicator_Size.large
                color=theme.colors.iconMediumEmphasis
              />
            </View>
          : React.null}
      </DocumentContext.ScrollView>
      {headerLeft->ReactUtils.mapOpt(headerLeft =>
        <View style={styles["headerLeft"]}> headerLeft </View>
      )}
      {headerRight->ReactUtils.mapOpt(headerRight =>
        <View style={styles["headerRight"]}> headerRight </View>
      )}
    </View>
  }
}

module Form = {
  @react.component
  let make = (
    ~headerLeft=?,
    ~headerRight=?,
    ~headerCenter=?,
    ~loading=?,
    ~style as styleFromProp=?,
    ~children,
  ) => {
    let {height} = Dimensions.useWindowDimensions()

    <Base
      ?headerLeft
      ?headerRight
      ?headerCenter
      ?loading
      style={
        open Style
        arrayOption([
          Some(style(~maxHeight=(height *. 0.9)->dp, ())),
          Some(styles["modalForm"]),
          styleFromProp,
        ])
      }>
      children
    </Base>
  }
}

module Large = {
  @react.component
  let make = (
    ~headerLeft=?,
    ~headerRight=?,
    ~headerCenter=?,
    ~loading=?,
    ~style as styleFromProp=?,
    ~children,
  ) => {
    let {height} = Dimensions.useWindowDimensions()

    <Base
      ?headerLeft
      ?headerRight
      ?headerCenter
      ?loading
      contentContainerStyle={styles["fullScreenHeight"]}
      style={
        open Style
        arrayOption([
          Some(style(~height=height->dp, ())),
          Some(styles["fullScreenWidth"]),
          styleFromProp,
        ])
      }>
      children
    </Base>
  }
}

module Dialog = {
  @react.component
  let make = (~children) => <Base style={styles["modalDialog"]}> children </Base>
}

include Base
