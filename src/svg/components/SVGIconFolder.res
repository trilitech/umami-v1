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
      d="M3.720 4.043 C 2.942 4.159,2.320 4.713,2.106 5.482 C 2.053 5.672,2.044 6.516,2.033 12.000 C 2.021 18.020,2.023 18.311,2.093 18.540 C 2.296 19.202,2.826 19.728,3.469 19.905 C 3.730 19.977,4.057 19.980,12.020 19.980 C 19.941 19.980,20.310 19.977,20.540 19.907 C 21.202 19.704,21.704 19.202,21.907 18.540 C 21.976 18.312,21.980 18.029,21.980 13.000 C 21.980 7.971,21.976 7.688,21.907 7.460 C 21.707 6.809,21.218 6.313,20.560 6.094 C 20.354 6.026,20.082 6.021,16.158 6.009 L 11.977 5.997 10.979 4.999 L 9.981 4.000 6.960 4.005 C 5.299 4.008,3.841 4.025,3.720 4.043 M10.180 7.000 L 11.179 8.000 15.590 8.000 L 20.000 8.000 20.000 13.000 L 20.000 18.000 12.000 18.000 L 4.000 18.000 4.000 12.000 L 4.000 6.000 6.590 6.000 L 9.181 6.000 10.180 7.000 "
      fillRule=#evenodd
    />
  </Svg>
