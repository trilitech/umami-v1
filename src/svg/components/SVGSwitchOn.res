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
  <Svg viewBox="0 0 34 24" ?width ?height ?fill ?stroke>
    <Path
      d="M5.980 6.291 C 3.386 6.557,1.392 8.337,0.847 10.874 C 0.719 11.467,0.719 12.533,0.847 13.126 C 1.353 15.479,3.117 17.201,5.471 17.639 C 5.857 17.711,6.205 17.715,12.224 17.715 C 18.159 17.715,18.594 17.711,18.949 17.642 C 21.301 17.190,23.056 15.471,23.560 13.126 C 23.687 12.533,23.687 11.467,23.560 10.874 C 23.057 8.537,21.322 6.830,18.970 6.359 C 18.636 6.292,18.168 6.287,12.366 6.282 C 8.932 6.279,6.058 6.283,5.980 6.291 M18.867 8.663 C 20.861 9.232,21.914 11.375,21.147 13.301 C 20.911 13.892,20.434 14.488,19.908 14.850 C 19.589 15.070,19.046 15.299,18.656 15.380 C 16.728 15.778,14.881 14.524,14.543 12.588 C 14.465 12.141,14.465 11.859,14.543 11.412 C 14.792 9.987,15.840 8.914,17.288 8.602 C 17.666 8.520,18.479 8.552,18.867 8.663 "
      fillRule=#evenodd
    />
  </Svg>
