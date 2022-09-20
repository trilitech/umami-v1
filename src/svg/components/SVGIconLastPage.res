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
      d="M16.000 12.000 L 16.000 18.000 17.000 18.000 L 18.000 18.000 18.000 12.000 L 18.000 6.000 17.000 6.000 L 16.000 6.000 16.000 12.000 M6.290 6.730 L 5.600 7.420 7.890 9.710 L 10.180 12.000 7.890 14.290 L 5.600 16.581 6.300 17.280 L 7.000 17.980 9.990 14.990 L 12.980 12.000 10.000 9.020 C 8.361 7.381,7.011 6.040,7.000 6.040 C 6.989 6.040,6.669 6.350,6.290 6.730 "
      fillRule=#evenodd
    />
  </Svg>
