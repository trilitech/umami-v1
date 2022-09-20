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
  <Svg viewBox="0 0 36 36" ?width ?height ?fill ?stroke>
    <Path
      d="M9.390 10.588 C 6.931 10.981,4.906 12.457,3.791 14.671 C 3.247 15.751,3.032 16.695,3.032 18.000 C 3.032 18.669,3.062 19.073,3.140 19.440 C 3.470 20.990,4.108 22.170,5.219 23.281 C 6.308 24.370,7.442 24.992,9.000 25.356 C 9.395 25.448,9.884 25.464,12.975 25.484 L 16.500 25.507 16.500 24.099 L 16.500 22.690 13.185 22.665 C 9.999 22.642,9.851 22.635,9.390 22.509 C 7.634 22.027,6.355 20.727,5.977 19.038 C 5.852 18.483,5.852 17.517,5.977 16.962 C 6.356 15.272,7.666 13.941,9.385 13.501 C 9.833 13.387,10.043 13.380,13.180 13.380 L 16.500 13.380 16.500 11.940 L 16.500 10.500 13.185 10.506 C 10.814 10.510,9.733 10.534,9.390 10.588 M19.500 11.937 L 19.500 13.380 22.803 13.380 C 26.470 13.380,26.565 13.388,27.410 13.775 C 29.104 14.551,30.120 16.136,30.120 18.000 C 30.120 20.172,28.759 21.919,26.610 22.509 C 26.149 22.635,26.001 22.642,22.815 22.665 L 19.500 22.690 19.500 24.099 L 19.500 25.507 23.025 25.484 C 26.116 25.464,26.605 25.448,27.000 25.356 C 28.459 25.015,29.622 24.404,30.636 23.444 C 31.315 22.801,31.797 22.142,32.224 21.270 C 33.416 18.839,33.220 16.087,31.690 13.770 C 31.328 13.223,30.265 12.160,29.722 11.802 C 28.885 11.251,28.017 10.881,27.000 10.644 C 26.605 10.552,26.116 10.536,23.025 10.516 L 19.500 10.493 19.500 11.937 M12.000 18.000 L 12.000 19.500 18.000 19.500 L 24.000 19.500 24.000 18.000 L 24.000 16.500 18.000 16.500 L 12.000 16.500 12.000 18.000 "
      fillRule=#evenodd
    />
  </Svg>
