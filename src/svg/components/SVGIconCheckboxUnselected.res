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
      d="M4.520 3.064 C 3.771 3.263,3.180 3.884,3.042 4.620 C 3.009 4.795,3.000 6.845,3.008 12.140 L 3.020 19.420 3.115 19.675 C 3.309 20.193,3.807 20.691,4.325 20.885 L 4.580 20.980 11.897 20.991 C 19.912 21.002,19.419 21.016,19.920 20.770 C 20.201 20.632,20.627 20.205,20.769 19.919 C 21.014 19.424,21.000 19.905,21.000 12.000 C 21.000 4.109,21.014 4.581,20.773 4.084 C 20.634 3.797,20.201 3.363,19.920 3.228 C 19.416 2.986,19.896 3.000,11.982 3.003 C 5.766 3.005,4.709 3.014,4.520 3.064 M19.000 12.000 L 19.000 19.000 12.000 19.000 L 5.000 19.000 5.000 12.000 L 5.000 5.000 12.000 5.000 L 19.000 5.000 19.000 12.000 "
      fillRule=#evenodd
    />
  </Svg>
