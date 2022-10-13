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

type springConfig = {
  delay: float,
  onUpdate: float => unit,
  onComplete: unit => unit,
}

@module("react-flip-toolkit")
external spring: springConfig => unit = "spring"

module Flipper = {
  @react.component @module("react-flip-toolkit")
  external make: (~flipKey: string, ~children: React.element) => React.element = "Flipper"
}

module Flipped = {
  type flippedProps

  @react.component @module("react-flip-toolkit")
  external make: (
    ~flipId: string,
    ~scale: bool=?,
    ~translate: bool=?,
    ~opacity: bool=?,
    ~onAppear: (Dom.element, int) => unit=?,
    ~onExit: (Dom.element, int, unit => unit) => unit=?,
    ~children: flippedProps => React.element,
  ) => React.element = "Flipped"

  module Inverse = {
    @react.component @module("react-flip-toolkit")
    external make: (
      ~inverseFlipId: string,
      ~children: flippedProps => React.element,
    ) => React.element = "Flipped"
  }
}

module ViewWithFlippedProps = {
  @react.component @module("./ViewWithFlippedProps")
  external make: (
    ~flippedProps: Flipped.flippedProps,
    ~zIndex: int=?,
    ~children: React.element,
  ) => React.element = "default"
}

module FlippedView = {
  @react.component
  let make = (
    ~flipId,
    ~scale=?,
    ~translate=?,
    ~opacity=?,
    ~onAppear=?,
    ~onExit=?,
    ~zIndex=?,
    ~children,
  ) =>
    <Flipped flipId ?scale ?translate ?opacity ?onAppear ?onExit>
      {flippedProps => <ViewWithFlippedProps flippedProps ?zIndex> children </ViewWithFlippedProps>}
    </Flipped>

  module Inverse = {
    @react.component
    let make = (~inverseFlipId: string, ~children: React.element) =>
      <Flipped.Inverse inverseFlipId>
        {flippedProps => <ViewWithFlippedProps flippedProps> children </ViewWithFlippedProps>}
      </Flipped.Inverse>
  }
}
