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

module Popper = {
  let styles = {
    open Style
    StyleSheet.create({
      "container": style(
        ~paddingVertical=4.->dp,
        ~paddingHorizontal=16.->dp,
        ~borderRadius=4.,
        ~borderWidth=1.,
        (),
      ),
      "positionLeft": style(~top=6.->dp, ~right=0.->dp, ~left="unset", ~position=#absolute, ()),
      "positionRight": style(~top=6.->dp, ~right="unset", ~left=0.->dp, ~position=#absolute, ()),
    })
  }

  @react.component
  let make = (~keyPopover, ~config: option<Popover.targetLayout>, ~text, ~isOpen) => {
    let theme = ThemeContext.useTheme()

    let dimensions = Dimensions.useWindowDimensions()

    let position =
      config->Option.mapWithDefault(styles["positionRight"], config =>
        config.x /. dimensions.width > 0.75 ? styles["positionLeft"] : styles["positionRight"]
      )

    <>
      <Popover openingStyle=Popover.Top style=position pointerEvents=#none keyPopover isOpen config>
        <View
          style={
            open Style
            array([
              styles["container"],
              style(
                ~backgroundColor=theme.colors.barBackground,
                ~borderColor=theme.colors.borderMediumEmphasis,
                (),
              ),
            ])
          }>
          <Typography.Overline3 numberOfLines=1 fontSize=12.>
            {text->React.string}
          </Typography.Overline3>
        </View>
      </Popover>
    </>
  }
}

@react.component
let make = (~pressableRef=?, ~style as styleFromProp=?, ~keyPopover, ~text, ~children) => {
  let (pressableRef, isOpen, popoverConfig, togglePopover, setClosed) = Popover.usePopoverState(
    ~elementRef=?pressableRef,
    (),
  )

  <View style=?styleFromProp onMouseEnter={_ => togglePopover()} onMouseLeave={_ => setClosed()}>
    {children(~pressableRef)} <Popper keyPopover text isOpen config=popoverConfig />
  </View>
}
