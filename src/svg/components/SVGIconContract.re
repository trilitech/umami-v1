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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.320 1.027 C 9.973 1.165,8.729 1.590,7.680 2.270 C 6.487 3.043,5.479 4.162,4.859 5.400 C 3.455 8.207,3.813 11.591,5.772 14.020 L 5.998 14.300 6.009 18.300 L 6.020 22.300 6.109 22.466 C 6.216 22.666,6.481 22.901,6.669 22.963 C 6.745 22.988,6.927 23.002,7.074 22.994 L 7.340 22.979 9.640 21.604 C 10.905 20.848,11.955 20.221,11.973 20.210 C 11.991 20.200,13.051 20.819,14.328 21.585 L 16.651 22.978 16.922 22.993 C 17.071 23.002,17.255 22.988,17.331 22.963 C 17.519 22.901,17.784 22.666,17.891 22.466 L 17.980 22.300 17.991 18.300 L 18.002 14.300 18.228 14.020 C 20.543 11.148,20.583 7.056,18.326 4.100 C 18.066 3.761,17.394 3.075,17.040 2.789 C 15.869 1.843,14.428 1.234,12.957 1.063 C 12.473 1.006,11.689 0.989,11.320 1.027 M12.635 3.060 C 13.513 3.125,14.504 3.484,15.333 4.039 C 16.923 5.102,17.904 6.853,17.987 8.776 C 18.101 11.435,16.430 13.860,13.906 14.700 C 13.184 14.940,12.912 14.980,12.000 14.980 C 11.360 14.980,11.105 14.964,10.840 14.908 C 8.663 14.443,6.953 12.914,6.287 10.835 C 6.073 10.166,6.023 9.818,6.025 9.000 C 6.026 8.161,6.091 7.763,6.343 7.034 C 7.110 4.820,9.195 3.213,11.520 3.043 C 11.931 3.013,12.031 3.015,12.635 3.060 M11.329 5.060 C 10.468 5.216,9.789 5.569,9.179 6.179 C 8.660 6.697,8.320 7.287,8.129 7.998 C 8.040 8.330,8.026 8.461,8.026 9.000 C 8.026 9.539,8.040 9.670,8.129 10.002 C 8.435 11.142,9.221 12.100,10.260 12.598 C 11.905 13.388,13.917 12.957,15.060 11.571 C 15.691 10.807,15.980 9.998,15.980 9.000 C 15.980 8.399,15.913 8.027,15.713 7.523 C 15.110 6.001,13.600 4.986,11.965 5.004 C 11.775 5.006,11.489 5.031,11.329 5.060 M12.622 6.581 C 13.385 6.776,14.023 7.343,14.329 8.100 C 14.455 8.410,14.459 8.439,14.459 9.000 C 14.459 9.561,14.455 9.591,14.330 9.900 C 14.071 10.544,13.544 11.071,12.900 11.330 C 12.591 11.455,12.561 11.459,12.000 11.459 C 11.439 11.459,11.410 11.455,11.100 11.329 C 10.343 11.023,9.776 10.385,9.581 9.622 C 9.502 9.313,9.503 8.684,9.582 8.378 C 9.807 7.511,10.496 6.816,11.360 6.584 C 11.653 6.506,12.320 6.504,12.622 6.581 M8.481 16.188 C 10.168 17.024,12.132 17.228,13.982 16.761 C 14.503 16.630,15.246 16.345,15.674 16.113 C 15.835 16.026,15.975 15.961,15.984 15.971 C 15.993 15.980,15.996 16.943,15.990 18.110 L 15.980 20.233 14.178 19.157 C 12.847 18.362,12.327 18.071,12.188 18.045 C 11.813 17.975,11.736 18.011,9.840 19.146 C 8.861 19.732,8.046 20.219,8.030 20.228 C 8.014 20.237,8.000 19.279,8.000 18.099 C 8.000 16.919,8.014 15.959,8.031 15.967 C 8.048 15.974,8.250 16.073,8.481 16.188 " fillRule=`evenodd> </Path> </Svg>;
