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
      d="M2.720 3.180 L 2.021 3.880 3.359 5.219 C 4.154 6.014,4.682 6.572,4.659 6.591 C 4.637 6.609,4.465 6.754,4.276 6.912 C 3.021 7.958,1.849 9.555,1.157 11.157 L 1.012 11.495 1.109 11.734 C 1.258 12.102,1.650 12.868,1.902 13.285 C 3.497 15.923,6.116 17.869,9.087 18.622 C 11.263 19.173,13.541 19.092,15.690 18.386 L 16.319 18.179 18.009 19.869 C 18.939 20.799,19.718 21.560,19.740 21.560 C 19.763 21.560,20.087 21.254,20.460 20.880 L 21.140 20.200 12.280 11.340 L 3.420 2.480 2.720 3.180 M11.180 4.041 C 10.367 4.100,9.101 4.334,8.550 4.528 L 8.400 4.581 9.212 5.392 L 10.024 6.203 10.362 6.145 C 11.336 5.976,12.615 5.958,13.520 6.100 C 16.059 6.499,18.359 7.889,19.879 9.940 C 20.177 10.342,20.529 10.903,20.713 11.265 L 20.834 11.506 20.660 11.832 C 20.209 12.676,19.598 13.489,18.894 14.183 L 18.445 14.626 19.143 15.323 L 19.841 16.020 20.373 15.480 C 21.448 14.389,22.260 13.193,22.843 11.843 L 22.988 11.505 22.891 11.266 C 22.742 10.899,22.350 10.132,22.098 9.715 C 21.117 8.092,19.683 6.662,18.060 5.690 C 16.012 4.464,13.538 3.871,11.180 4.041 M11.680 7.027 C 11.379 7.058,11.000 7.129,11.000 7.154 C 11.000 7.215,13.073 9.235,13.220 9.318 C 13.628 9.548,13.993 9.916,14.192 10.300 C 14.238 10.390,14.726 10.910,15.311 11.493 L 16.348 12.527 16.395 12.293 C 16.583 11.374,16.445 10.396,15.999 9.500 C 15.303 8.102,14.031 7.218,12.460 7.041 C 12.106 7.001,11.958 6.999,11.680 7.027 M7.753 10.053 C 7.431 11.018,7.448 12.128,7.800 13.070 C 8.600 15.207,10.905 16.406,13.126 15.838 C 13.303 15.793,13.519 15.728,13.608 15.692 L 13.768 15.628 14.268 16.128 L 14.768 16.628 14.428 16.714 C 13.107 17.045,11.468 17.091,10.126 16.835 C 7.450 16.323,5.067 14.673,3.631 12.337 C 3.509 12.138,3.354 11.867,3.287 11.735 L 3.166 11.494 3.343 11.164 C 3.897 10.126,4.766 9.069,5.718 8.273 L 6.097 7.957 6.980 8.840 L 7.864 9.723 7.753 10.053 M10.840 12.700 L 12.140 14.000 11.900 13.999 C 11.010 13.997,10.109 13.394,9.717 12.538 C 9.552 12.177,9.411 11.400,9.510 11.400 C 9.527 11.400,10.125 11.985,10.840 12.700 "
      fillRule=#evenodd
    />
  </Svg>
