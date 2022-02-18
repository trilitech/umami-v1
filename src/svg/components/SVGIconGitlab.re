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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.806 3.046 C 5.687 3.073,5.577 3.140,5.461 3.257 L 5.291 3.427 3.657 8.323 C 1.837 13.778,1.916 13.486,2.131 13.926 C 2.190 14.045,2.287 14.189,2.349 14.247 C 2.569 14.453,11.773 20.931,11.902 20.971 C 12.009 21.004,12.063 20.997,12.187 20.934 C 12.369 20.842,21.490 14.398,21.651 14.247 C 21.713 14.189,21.810 14.045,21.869 13.926 C 22.084 13.486,22.163 13.778,20.343 8.320 L 18.709 3.420 18.540 3.256 C 18.349 3.071,18.084 2.982,17.847 3.023 C 17.649 3.058,17.387 3.212,17.300 3.346 C 17.260 3.406,16.765 4.833,16.198 6.518 L 15.169 9.580 11.998 9.580 L 8.828 9.580 7.814 6.545 C 7.256 4.875,6.762 3.449,6.717 3.374 C 6.550 3.104,6.159 2.963,5.806 3.046 M6.955 6.880 C 7.442 8.343,7.840 9.554,7.840 9.570 C 7.840 9.587,7.030 9.600,6.040 9.600 C 4.772 9.600,4.240 9.587,4.240 9.557 C 4.240 9.447,6.013 4.157,6.040 4.187 C 6.056 4.205,6.468 5.417,6.955 6.880 M18.897 6.872 C 19.389 8.348,19.782 9.565,19.769 9.577 C 19.735 9.612,16.160 9.604,16.160 9.570 C 16.160 9.545,17.873 4.382,17.941 4.204 C 17.952 4.174,17.971 4.158,17.982 4.169 C 17.994 4.180,18.405 5.397,18.897 6.872 M8.232 10.690 C 8.405 11.161,10.727 18.166,10.714 18.179 C 10.697 18.196,4.560 10.544,4.560 10.506 C 4.560 10.492,5.369 10.480,6.357 10.480 L 8.154 10.480 8.232 10.690 M13.447 14.760 C 12.662 17.114,12.011 19.040,12.000 19.040 C 11.989 19.040,11.338 17.114,10.553 14.760 L 9.127 10.480 12.000 10.480 L 14.873 10.480 13.447 14.760 M19.313 10.697 C 17.288 13.236,13.296 18.190,13.285 18.178 C 13.277 18.170,13.850 16.434,14.560 14.322 L 15.851 10.480 17.668 10.480 L 19.486 10.480 19.313 10.697 M6.667 14.611 C 8.255 16.595,9.549 18.224,9.542 18.231 C 9.514 18.260,3.015 13.634,2.964 13.549 C 2.913 13.463,2.943 13.352,3.317 12.228 C 3.543 11.552,3.739 11.001,3.753 11.001 C 3.768 11.002,5.079 12.627,6.667 14.611 M20.683 12.228 C 21.058 13.353,21.087 13.463,21.036 13.550 C 20.985 13.635,14.487 18.260,14.458 18.231 C 14.441 18.214,20.214 11.005,20.247 11.002 C 20.261 11.001,20.457 11.552,20.683 12.228 " fillRule=`evenodd> </Path> </Svg>;
