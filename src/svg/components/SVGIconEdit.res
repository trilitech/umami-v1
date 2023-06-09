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
      d="M17.400 3.037 C 17.168 3.108,17.014 3.241,16.100 4.157 L 15.140 5.120 17.010 6.990 L 18.880 8.860 19.859 7.881 C 20.954 6.787,21.022 6.690,20.992 6.283 C 20.983 6.160,20.942 5.988,20.901 5.900 C 20.852 5.795,20.380 5.293,19.523 4.436 C 18.368 3.280,18.198 3.124,18.023 3.066 C 17.817 2.997,17.570 2.986,17.400 3.037 M8.530 11.730 L 3.000 17.260 3.000 19.130 L 3.000 21.000 4.870 21.000 L 6.740 21.000 12.270 15.470 L 17.800 9.940 15.930 8.070 L 14.060 6.200 8.530 11.730 M14.540 9.480 L 14.979 9.921 10.440 14.460 L 5.900 19.000 5.450 19.000 L 5.000 19.000 5.000 18.530 L 5.000 18.060 9.510 13.550 C 11.990 11.070,14.038 9.040,14.061 9.040 C 14.083 9.040,14.299 9.238,14.540 9.480 "
      fillRule=#evenodd
    />
  </Svg>
