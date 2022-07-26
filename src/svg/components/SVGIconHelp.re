/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com>          */
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
    ) =>
  <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke>
    <Path
      d="M11.260 2.023 C 11.194 2.032,10.969 2.059,10.760 2.083 C 9.544 2.221,8.182 2.671,7.091 3.294 C 4.825 4.588,3.174 6.650,2.427 9.120 C 2.131 10.102,2.021 10.880,2.021 12.000 C 2.021 13.120,2.131 13.898,2.427 14.880 C 3.375 18.015,5.773 20.469,8.900 21.501 C 9.953 21.849,10.809 21.980,12.020 21.978 C 13.655 21.975,14.974 21.674,16.380 20.980 C 19.438 19.473,21.449 16.666,21.926 13.240 C 21.995 12.744,21.995 11.256,21.926 10.760 C 21.694 9.093,21.115 7.595,20.194 6.280 C 18.618 4.029,16.212 2.532,13.500 2.115 C 13.007 2.039,11.569 1.980,11.260 2.023 M9.689 5.870 C 9.991 6.590,10.249 7.209,10.261 7.244 C 10.279 7.294,10.181 7.359,9.830 7.531 C 8.813 8.029,8.050 8.789,7.537 9.813 C 7.414 10.059,7.300 10.260,7.284 10.260 C 7.268 10.260,6.648 10.008,5.906 9.700 C 4.339 9.050,4.476 9.204,4.829 8.482 C 5.635 6.837,7.025 5.484,8.680 4.736 C 8.889 4.642,9.078 4.563,9.100 4.562 C 9.122 4.561,9.387 5.150,9.689 5.870 M15.268 4.715 C 16.723 5.356,18.032 6.520,18.873 7.924 C 19.114 8.324,19.395 8.910,19.410 9.041 C 19.419 9.126,19.316 9.176,18.086 9.688 C 17.352 9.993,16.735 10.247,16.716 10.251 C 16.696 10.256,16.619 10.121,16.544 9.950 C 16.094 8.922,15.151 7.973,14.110 7.499 C 13.918 7.412,13.761 7.321,13.760 7.298 C 13.760 7.264,14.153 6.299,14.822 4.690 C 14.852 4.619,14.885 4.560,14.896 4.560 C 14.907 4.560,15.075 4.630,15.268 4.715 M12.780 9.101 C 13.794 9.377,14.623 10.211,14.906 11.240 C 15.004 11.599,15.004 12.401,14.906 12.760 C 14.621 13.797,13.797 14.621,12.760 14.906 C 12.575 14.957,12.356 14.977,12.000 14.977 C 11.245 14.977,10.691 14.789,10.137 14.345 C 9.264 13.645,8.842 12.508,9.058 11.439 C 9.301 10.233,10.184 9.330,11.380 9.064 C 11.725 8.987,12.431 9.005,12.780 9.101 M7.466 14.065 C 7.777 14.780,8.473 15.622,9.120 16.065 C 9.413 16.265,9.823 16.489,10.134 16.616 C 10.227 16.654,10.281 16.700,10.269 16.730 C 10.122 17.105,9.146 19.389,9.122 19.413 C 9.039 19.498,7.993 18.949,7.269 18.440 C 6.793 18.105,5.896 17.209,5.560 16.731 C 5.221 16.250,4.810 15.534,4.664 15.170 C 4.600 15.010,4.555 14.873,4.564 14.866 C 4.609 14.828,7.302 13.731,7.317 13.744 C 7.326 13.753,7.393 13.897,7.466 14.065 M18.064 14.293 C 19.621 14.951,19.499 14.837,19.239 15.391 C 18.466 17.041,17.041 18.466,15.391 19.239 C 14.837 19.499,14.951 19.622,14.302 18.060 L 13.737 16.700 14.188 16.475 C 15.181 15.980,15.946 15.225,16.434 14.257 C 16.523 14.080,16.615 13.886,16.637 13.827 C 16.660 13.768,16.685 13.720,16.693 13.720 C 16.701 13.720,17.318 13.978,18.064 14.293 "
      fillRule=`evenodd
    />
  </Svg>;
