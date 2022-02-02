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
    ) => <Svg viewBox="0 0 36 36" ?width ?height ?fill ?stroke> <Path d="M16.760 6.066 C 13.714 6.421,10.895 8.022,9.064 10.436 C 8.683 10.940,8.349 11.449,8.165 11.811 C 8.047 12.041,8.030 12.051,7.607 12.119 C 3.862 12.720,0.980 15.488,0.172 19.260 C -0.022 20.169,-0.022 21.831,0.172 22.740 C 0.520 24.361,1.234 25.803,2.249 26.930 C 3.630 28.463,5.216 29.382,7.260 29.831 L 7.890 29.970 18.660 29.970 C 28.849 29.970,29.457 29.964,29.940 29.861 C 31.811 29.460,33.404 28.454,34.508 26.976 C 34.940 26.398,35.526 25.229,35.700 24.600 C 36.208 22.759,36.056 20.948,35.254 19.290 C 34.510 17.749,33.412 16.629,31.902 15.869 C 31.154 15.491,30.524 15.287,29.649 15.137 L 29.027 15.030 28.889 14.490 C 28.060 11.248,25.880 8.573,22.897 7.135 C 20.987 6.214,18.781 5.830,16.760 6.066 M19.650 9.169 C 22.546 9.791,24.914 11.902,25.822 14.670 C 25.908 14.934,26.101 15.755,26.249 16.495 C 26.398 17.234,26.532 17.852,26.547 17.867 C 26.562 17.882,27.109 17.932,27.762 17.979 C 29.295 18.087,29.778 18.181,30.450 18.502 C 31.613 19.058,32.433 20.023,32.828 21.300 C 32.940 21.663,32.967 21.889,32.968 22.470 C 32.970 23.230,32.905 23.588,32.655 24.206 C 32.239 25.234,31.237 26.242,30.229 26.647 C 29.370 26.991,30.222 26.966,18.933 26.985 C 9.836 27.000,8.491 26.991,8.024 26.910 C 6.734 26.688,5.685 26.147,4.768 25.231 C 3.991 24.455,3.479 23.569,3.193 22.503 C 3.060 22.005,3.040 21.809,3.040 21.000 C 3.040 20.192,3.060 19.995,3.193 19.499 C 3.562 18.123,4.306 17.018,5.423 16.187 C 6.337 15.508,7.227 15.178,8.550 15.029 C 8.979 14.981,9.470 14.926,9.642 14.907 L 9.954 14.872 10.413 13.991 C 11.023 12.821,11.426 12.225,12.046 11.573 C 12.742 10.842,13.438 10.332,14.370 9.869 C 15.198 9.458,15.891 9.233,16.770 9.091 C 17.494 8.974,18.919 9.012,19.650 9.169 M14.970 18.000 L 11.970 21.000 13.905 21.000 L 15.840 21.000 15.840 23.250 L 15.840 25.500 18.000 25.500 L 20.160 25.500 20.160 23.250 L 20.160 21.000 22.095 21.000 L 24.030 21.000 21.030 18.000 C 19.380 16.350,18.017 15.000,18.000 15.000 C 17.983 15.000,16.620 16.350,14.970 18.000 " fillRule=`evenodd> </Path> </Svg>;
