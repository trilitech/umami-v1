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

let listVerticalPadding = 8.

let styles = {
  open Style
  StyleSheet.create({
    "listContainer": style(~borderRadius=3., ~maxHeight=224.->dp, ())->unsafeAddStyle({
      "boxShadow": "0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 3px 14px 2px rgba(0, 0, 0, 0.12), 0 8px 10px 1px rgba(0, 0, 0, 0.14)",
    }),
    "positionBottomTop": style(~bottom=35.->dp, ~top="unset", ~position=#absolute, ()),
    "listContentContainer": style(~paddingVertical=listVerticalPadding->dp, ()),
  })
}

@react.component
let make = (
  ~scrollRef=?,
  ~isOpen,
  ~popoverConfig: option<Popover.targetLayout>,
  ~openingStyle=Popover.Top,
  ~style as styleFromProp=?,
  ~onScroll=?,
  ~scrollEventThrottle=?,
  ~onRequestClose: unit => unit,
  ~keyPopover,
  ~reversePositionPct=0.80,
  ~children: bool => array<React.element>,
) => {
  let theme = ThemeContext.useTheme()

  let dimensions = Dimensions.useWindowDimensions()

  let positionReversed =
    popoverConfig->Option.flatMap(config =>
      config.y /. dimensions.height > reversePositionPct ? Some(styles["positionBottomTop"]) : None
    )

  <Popover
    isOpen
    openingStyle
    config=popoverConfig
    style={Style.arrayOption([styleFromProp, positionReversed])}
    keyPopover>
    <View
      style={
        open Style
        style(~backgroundColor=theme.colors.background, ())
      }
      onStartShouldSetResponderCapture={_ => true}
      onResponderRelease={_ => onRequestClose()}>
      <ScrollView
        ref=?scrollRef
        style={
          open Style
          array([
            styles["listContainer"],
            theme.dark
              ? style(~backgroundColor=theme.colors.stateActive, ())
              : style(~backgroundColor=theme.colors.background, ()),
          ])
        }
        contentContainerStyle=?Some(styles["listContentContainer"])
        ?onScroll
        ?scrollEventThrottle>
        {children(positionReversed != None)
        ->(a => positionReversed == None ? a : a->Array.reverse)
        ->React.array}
      </ScrollView>
    </View>
  </Popover>
}
