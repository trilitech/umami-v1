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
      d="M9.000 3.500 L 8.501 4.000 6.751 4.000 L 5.000 4.000 5.000 5.000 L 5.000 6.000 12.000 6.000 L 19.000 6.000 19.000 5.000 L 19.000 4.000 17.249 4.000 L 15.499 4.000 15.000 3.500 L 14.501 3.000 12.000 3.000 L 9.499 3.000 9.000 3.500 M6.009 13.210 L 6.020 19.420 6.115 19.675 C 6.309 20.193,6.807 20.691,7.325 20.885 L 7.580 20.980 11.896 20.991 C 16.617 21.003,16.429 21.011,16.920 20.770 C 17.201 20.632,17.627 20.205,17.769 19.919 C 18.013 19.428,18.000 19.796,18.000 13.112 L 18.000 7.000 11.999 7.000 L 5.999 7.000 6.009 13.210 M16.000 14.000 L 16.000 19.000 12.000 19.000 L 8.000 19.000 8.000 14.000 L 8.000 9.000 12.000 9.000 L 16.000 9.000 16.000 14.000 "
      fillRule=#evenodd
    />
  </Svg>
