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
      d="M11.240 2.039 C 9.936 2.141,8.683 2.494,7.500 3.092 C 5.551 4.078,4.024 5.623,3.043 7.600 C 2.350 8.999,2.029 10.384,2.028 11.980 C 2.027 13.603,2.348 14.996,3.046 16.404 C 4.040 18.411,5.592 19.962,7.594 20.953 C 10.375 22.329,13.631 22.329,16.404 20.954 C 18.412 19.959,19.959 18.412,20.954 16.404 C 21.690 14.921,22.059 13.191,21.978 11.610 L 21.950 11.080 20.964 11.080 L 19.979 11.080 19.994 11.779 C 20.016 12.817,19.901 13.620,19.601 14.518 C 18.665 17.313,16.338 19.330,13.400 19.891 C 12.783 20.008,11.272 20.018,10.689 19.907 C 8.961 19.580,7.498 18.823,6.334 17.653 C 5.161 16.474,4.419 15.036,4.093 13.311 C 3.982 12.728,3.992 11.217,4.109 10.600 C 4.669 7.667,6.693 5.328,9.471 4.404 C 10.406 4.093,11.193 3.981,12.266 4.008 L 13.000 4.026 13.000 3.056 C 13.000 2.522,12.986 2.076,12.970 2.066 C 12.907 2.027,11.644 2.008,11.240 2.039 M17.000 3.580 L 17.000 5.080 15.500 5.080 L 14.000 5.080 14.000 6.060 L 14.000 7.040 15.500 7.040 L 17.000 7.040 17.000 8.540 L 17.000 10.040 18.000 10.040 L 19.000 10.040 19.000 8.540 L 19.000 7.040 20.500 7.040 L 22.000 7.040 22.000 6.060 L 22.000 5.080 20.500 5.080 L 19.000 5.080 19.000 3.580 L 19.000 2.080 18.000 2.080 L 17.000 2.080 17.000 3.580 M8.843 8.590 C 8.767 8.629,8.668 8.711,8.623 8.773 C 8.541 8.885,8.540 8.913,8.540 12.000 C 8.540 15.087,8.541 15.115,8.623 15.227 C 8.668 15.289,8.767 15.371,8.843 15.410 C 8.970 15.475,9.204 15.480,11.994 15.480 C 15.326 15.480,15.181 15.491,15.371 15.225 C 15.460 15.100,15.460 15.097,15.471 13.090 L 15.482 11.080 14.561 11.080 L 13.640 11.080 13.640 12.218 C 13.640 13.423,13.627 13.516,13.454 13.596 C 13.388 13.625,12.909 13.640,12.000 13.640 C 10.566 13.640,10.485 13.631,10.404 13.454 C 10.375 13.388,10.360 12.909,10.360 12.000 C 10.360 11.091,10.375 10.612,10.404 10.546 C 10.484 10.372,10.575 10.360,11.822 10.360 L 13.000 10.360 13.000 9.440 L 13.000 8.520 10.990 8.520 C 9.155 8.520,8.968 8.526,8.843 8.590 "
      fillRule=#evenodd
    />
  </Svg>
