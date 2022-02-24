/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ReactNative;
open ReactNativeSvg;

[@react.component]
let make =
    (
      ~width: option(Style.size)=?,
      ~height: option(Style.size)=?,
      ~fill: option(string)=?,
      ~stroke: option(string)=?,
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M3.995 1.042 C 3.135 1.170,2.426 1.748,2.129 2.562 L 2.020 2.860 2.020 10.680 L 2.020 18.500 2.116 18.793 C 2.343 19.486,2.909 20.042,3.640 20.291 L 3.900 20.380 10.593 20.391 C 16.980 20.401,17.285 20.398,17.265 20.331 C 16.906 19.116,16.710 18.461,16.690 18.408 C 16.654 18.314,16.719 18.370,18.325 19.802 C 19.449 20.805,21.531 22.603,21.931 22.916 C 21.998 22.968,22.001 22.520,21.991 12.915 L 21.980 2.860 21.876 2.576 C 21.611 1.853,21.046 1.327,20.278 1.089 C 20.077 1.027,19.452 1.021,12.140 1.015 C 7.784 1.012,4.119 1.024,3.995 1.042 M10.321 6.181 C 10.349 6.214,10.364 6.248,10.356 6.256 C 10.347 6.264,10.088 6.355,9.780 6.458 C 9.154 6.669,8.329 7.051,7.890 7.333 C 7.731 7.436,7.600 7.527,7.600 7.537 C 7.600 7.547,7.792 7.474,8.027 7.376 C 9.210 6.880,10.205 6.665,11.700 6.580 C 13.046 6.504,14.775 6.844,16.058 7.437 C 16.246 7.524,16.400 7.589,16.400 7.582 C 16.400 7.563,15.800 7.182,15.560 7.049 C 15.256 6.881,14.559 6.588,14.104 6.437 L 13.723 6.311 13.792 6.213 C 13.857 6.119,13.873 6.116,14.185 6.137 C 14.925 6.188,15.959 6.547,16.700 7.010 L 17.100 7.260 17.311 7.680 C 18.047 9.148,18.568 11.039,18.718 12.780 C 18.793 13.655,18.791 13.666,18.550 13.952 C 17.989 14.616,17.037 15.122,16.012 15.300 C 15.823 15.333,15.568 15.360,15.445 15.360 L 15.220 15.360 14.848 14.919 C 14.613 14.641,14.495 14.472,14.528 14.461 C 15.360 14.173,15.877 13.876,16.298 13.444 L 16.496 13.241 15.998 13.491 C 15.127 13.928,14.248 14.206,13.293 14.346 C 12.735 14.427,11.596 14.437,11.080 14.364 C 10.116 14.228,9.132 13.929,8.320 13.528 C 8.023 13.381,7.762 13.254,7.740 13.245 C 7.662 13.215,8.023 13.570,8.247 13.744 C 8.532 13.965,8.989 14.213,9.334 14.334 C 9.480 14.385,9.611 14.438,9.624 14.451 C 9.637 14.464,9.480 14.674,9.275 14.917 L 8.902 15.360 8.676 15.360 C 8.373 15.360,7.839 15.264,7.433 15.136 C 6.763 14.924,6.143 14.542,5.681 14.056 L 5.400 13.760 5.400 13.318 C 5.400 11.710,5.940 9.546,6.790 7.750 L 7.012 7.280 7.316 7.085 C 8.033 6.626,8.834 6.308,9.597 6.179 C 10.027 6.107,10.260 6.107,10.321 6.181 M9.595 10.295 C 9.097 10.473,8.799 10.891,8.795 11.418 C 8.791 11.920,9.029 12.328,9.448 12.537 C 9.694 12.659,10.042 12.675,10.328 12.578 C 10.891 12.386,11.207 11.696,11.022 11.061 C 10.929 10.741,10.591 10.396,10.279 10.304 C 10.024 10.228,9.792 10.225,9.595 10.295 M13.720 10.316 C 13.434 10.417,13.118 10.745,13.020 11.041 C 12.861 11.520,13.025 12.100,13.406 12.401 C 13.637 12.584,13.810 12.640,14.140 12.639 C 14.577 12.638,14.946 12.385,15.139 11.955 C 15.238 11.735,15.248 11.674,15.232 11.384 C 15.211 11.014,15.125 10.808,14.893 10.576 C 14.594 10.277,14.124 10.173,13.720 10.316 " fillRule=`evenodd> </Path> </Svg>;
