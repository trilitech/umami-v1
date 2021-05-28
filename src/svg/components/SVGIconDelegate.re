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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.901 3.045 C 4.008 3.320,2.523 4.682,2.090 6.539 C 2.043 6.740,2.025 7.002,2.026 7.460 C 2.028 8.027,2.040 8.146,2.136 8.499 C 2.485 9.784,3.335 10.804,4.545 11.388 C 4.790 11.506,5.159 11.645,5.365 11.697 L 5.740 11.792 5.760 16.213 C 5.780 20.606,5.781 20.635,5.863 20.747 C 5.908 20.809,6.007 20.891,6.083 20.930 C 6.212 20.996,6.576 21.000,11.991 21.000 C 18.250 21.000,17.908 21.012,18.120 20.782 L 18.220 20.674 18.240 16.231 L 18.260 11.788 18.571 11.715 C 19.879 11.406,21.023 10.468,21.598 9.232 C 22.188 7.964,22.111 6.423,21.398 5.237 C 20.058 3.007,17.183 2.332,14.971 3.729 L 14.723 3.886 14.549 3.766 C 14.453 3.700,14.232 3.575,14.058 3.488 C 13.026 2.974,11.842 2.865,10.729 3.183 C 10.344 3.293,9.738 3.569,9.456 3.763 L 9.274 3.888 9.063 3.748 C 8.181 3.162,6.960 2.890,5.901 3.045 M7.900 5.017 C 8.264 5.119,8.676 5.332,9.004 5.589 C 9.283 5.807,9.312 5.820,9.520 5.820 C 9.724 5.820,9.760 5.805,10.011 5.611 C 10.388 5.320,10.594 5.206,10.994 5.067 C 11.290 4.964,11.415 4.945,11.862 4.929 C 12.303 4.913,12.436 4.923,12.722 4.995 C 13.151 5.103,13.638 5.336,13.924 5.569 C 14.212 5.803,14.284 5.840,14.460 5.840 C 14.635 5.840,14.681 5.817,15.029 5.553 C 15.372 5.293,15.767 5.104,16.198 4.996 C 16.647 4.882,17.429 4.903,17.860 5.039 C 18.896 5.367,19.682 6.194,19.903 7.189 C 20.008 7.660,19.968 8.324,19.808 8.751 C 19.582 9.357,19.089 9.953,18.548 10.275 C 18.225 10.466,17.699 10.637,17.270 10.690 C 16.943 10.730,16.887 10.749,16.761 10.864 L 16.620 10.994 16.600 15.047 L 16.580 19.100 15.650 19.111 L 14.720 19.121 14.720 16.331 C 14.720 13.219,14.732 13.363,14.444 13.171 C 14.280 13.061,14.106 13.055,13.919 13.153 C 13.629 13.307,13.640 13.178,13.640 16.250 L 13.640 19.000 13.080 19.000 L 12.520 19.000 12.520 16.230 C 12.520 13.522,12.518 13.457,12.439 13.328 C 12.296 13.094,11.967 13.015,11.732 13.158 C 11.444 13.334,11.461 13.145,11.449 16.228 L 11.438 19.000 10.879 19.000 L 10.320 19.000 10.320 16.205 C 10.320 13.432,10.319 13.410,10.236 13.305 C 10.001 13.005,9.551 13.017,9.361 13.328 C 9.282 13.457,9.280 13.522,9.280 16.230 L 9.280 19.000 8.340 19.000 L 7.400 19.000 7.400 15.079 C 7.400 10.770,7.411 10.958,7.157 10.804 C 7.087 10.762,6.920 10.715,6.785 10.700 C 5.934 10.607,5.166 10.220,4.682 9.639 C 3.936 8.742,3.799 7.614,4.307 6.561 C 4.463 6.239,4.544 6.128,4.855 5.818 C 5.162 5.511,5.281 5.423,5.606 5.262 C 6.172 4.981,6.462 4.918,7.120 4.936 C 7.462 4.945,7.748 4.974,7.900 5.017 " fillRule=`evenodd> </Path> </Svg>;
