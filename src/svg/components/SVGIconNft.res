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
      d="M6.259 4.042 C 6.192 4.062,6.082 4.137,6.013 4.209 C 5.771 4.462,2.504 9.060,2.449 9.225 C 2.378 9.437,2.433 9.656,2.611 9.869 C 2.682 9.954,4.735 12.240,7.174 14.949 C 10.180 18.287,11.649 19.891,11.734 19.926 C 11.895 19.993,12.105 19.992,12.268 19.925 C 12.356 19.889,13.785 18.330,16.848 14.928 C 19.296 12.209,21.349 9.923,21.410 9.847 C 21.570 9.647,21.619 9.429,21.551 9.225 C 21.499 9.066,18.290 4.545,18.007 4.231 C 17.930 4.146,17.809 4.064,17.726 4.041 C 17.542 3.990,6.434 3.991,6.259 4.042 M10.639 5.310 C 10.638 5.345,8.485 8.015,8.446 8.030 C 8.419 8.040,7.400 5.405,7.400 5.325 C 7.400 5.293,7.854 5.280,9.020 5.280 C 9.911 5.280,10.640 5.294,10.639 5.310 M16.601 5.310 C 16.603 5.362,15.581 8.039,15.560 8.038 C 15.537 8.036,13.387 5.367,13.367 5.315 C 13.359 5.295,14.076 5.280,14.977 5.280 C 15.870 5.280,16.601 5.294,16.601 5.310 M13.259 7.220 L 14.486 8.740 13.243 8.751 C 12.559 8.756,11.440 8.756,10.755 8.751 L 9.510 8.740 10.741 7.209 C 11.418 6.366,11.986 5.682,12.003 5.689 C 12.019 5.695,12.585 6.384,13.259 7.220 M6.778 7.250 C 7.040 7.938,7.279 8.559,7.308 8.630 L 7.361 8.760 5.823 8.760 L 4.285 8.760 5.273 7.381 C 5.816 6.622,6.269 6.001,6.281 6.001 C 6.292 6.000,6.516 6.563,6.778 7.250 M18.735 7.390 L 19.717 8.760 18.182 8.760 L 16.648 8.760 16.905 8.090 C 17.046 7.721,17.282 7.100,17.429 6.710 C 17.577 6.319,17.710 6.004,17.726 6.010 C 17.741 6.015,18.195 6.636,18.735 7.390 M9.083 13.279 C 9.761 15.049,10.310 16.503,10.304 16.509 C 10.294 16.519,4.824 10.457,4.566 10.151 L 4.472 10.039 6.161 10.050 L 7.850 10.060 9.083 13.279 M14.782 10.110 C 14.767 10.149,14.140 11.787,13.390 13.750 C 12.639 15.713,12.014 17.320,12.000 17.320 C 11.983 17.320,9.370 10.552,9.209 10.090 C 9.195 10.050,9.771 10.040,12.000 10.040 C 14.658 10.040,14.807 10.044,14.782 10.110 M19.473 10.110 C 19.312 10.299,13.931 16.279,13.813 16.400 L 13.677 16.540 13.775 16.280 C 13.846 16.089,15.857 10.826,16.123 10.130 L 16.158 10.040 17.845 10.040 C 19.448 10.040,19.529 10.043,19.473 10.110 "
      fillRule=#evenodd
    />
  </Svg>
