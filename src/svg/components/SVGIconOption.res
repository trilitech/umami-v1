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
      d="M15.000 6.000 L 15.000 9.000 16.000 9.000 L 17.000 9.000 17.000 8.000 L 17.000 7.000 19.000 7.000 L 21.000 7.000 21.000 6.000 L 21.000 5.000 19.000 5.000 L 17.000 5.000 17.000 4.000 L 17.000 3.000 16.000 3.000 L 15.000 3.000 15.000 6.000 M3.000 6.000 L 3.000 7.000 8.000 7.000 L 13.000 7.000 13.000 6.000 L 13.000 5.000 8.000 5.000 L 3.000 5.000 3.000 6.000 M7.000 10.000 L 7.000 11.000 5.000 11.000 L 3.000 11.000 3.000 12.000 L 3.000 13.000 5.000 13.000 L 7.000 13.000 7.000 14.000 L 7.000 15.000 8.000 15.000 L 9.000 15.000 9.000 12.000 L 9.000 9.000 8.000 9.000 L 7.000 9.000 7.000 10.000 M11.000 12.000 L 11.000 13.000 16.000 13.000 L 21.000 13.000 21.000 12.000 L 21.000 11.000 16.000 11.000 L 11.000 11.000 11.000 12.000 M11.000 18.000 L 11.000 21.000 12.000 21.000 L 13.000 21.000 13.000 20.000 L 13.000 19.000 17.000 19.000 L 21.000 19.000 21.000 18.000 L 21.000 17.000 17.000 17.000 L 13.000 17.000 13.000 16.000 L 13.000 15.000 12.000 15.000 L 11.000 15.000 11.000 18.000 M3.000 18.000 L 3.000 19.000 6.000 19.000 L 9.000 19.000 9.000 18.000 L 9.000 17.000 6.000 17.000 L 3.000 17.000 3.000 18.000 "
      fillRule=#evenodd
    />
  </Svg>
