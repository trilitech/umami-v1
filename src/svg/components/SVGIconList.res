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
open ReactNativeSvg

@react.component
let make = (
  ~width: option<Style.size>=?,
  ~height: option<Style.size>=?,
  ~fill: option<string>=?,
  ~stroke: option<string>=?,
) =>
  <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke>
    <Path
      d="M3.000 8.000 L 3.000 9.000 4.000 9.000 L 5.000 9.000 5.000 8.000 L 5.000 7.000 4.000 7.000 L 3.000 7.000 3.000 8.000 M7.000 8.000 L 7.000 9.000 14.000 9.000 L 21.000 9.000 21.000 8.000 L 21.000 7.000 14.000 7.000 L 7.000 7.000 7.000 8.000 M3.000 12.000 L 3.000 13.000 4.000 13.000 L 5.000 13.000 5.000 12.000 L 5.000 11.000 4.000 11.000 L 3.000 11.000 3.000 12.000 M7.000 12.000 L 7.000 13.000 14.000 13.000 L 21.000 13.000 21.000 12.000 L 21.000 11.000 14.000 11.000 L 7.000 11.000 7.000 12.000 M3.000 16.000 L 3.000 17.000 4.000 17.000 L 5.000 17.000 5.000 16.000 L 5.000 15.000 4.000 15.000 L 3.000 15.000 3.000 16.000 M7.000 16.000 L 7.000 17.000 14.000 17.000 L 21.000 17.000 21.000 16.000 L 21.000 15.000 14.000 15.000 L 7.000 15.000 7.000 16.000 "
      fillRule=#evenodd
    />
  </Svg>
