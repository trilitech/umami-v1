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
    ) => <Svg viewBox="0 0 20 20" ?width ?height ?fill ?stroke> <Path d="M9.367 1.686 C 8.032 1.838,7.148 2.092,6.167 2.605 C 5.372 3.021,4.758 3.473,4.116 4.116 C 3.422 4.809,2.952 5.466,2.516 6.350 C 0.712 10.011,1.869 14.473,5.233 16.828 C 6.330 17.596,7.579 18.079,8.967 18.271 C 9.436 18.337,10.653 18.327,11.150 18.255 C 12.386 18.075,13.580 17.620,14.601 16.938 C 16.650 15.569,17.924 13.515,18.273 11.017 C 18.328 10.622,18.327 9.372,18.271 8.967 C 17.878 6.118,16.190 3.762,13.650 2.516 C 12.515 1.959,11.462 1.708,10.167 1.685 C 9.791 1.678,9.431 1.679,9.367 1.686 M10.783 4.263 C 11.997 4.566,12.940 5.509,13.236 6.717 C 13.432 7.515,13.310 8.315,12.898 8.943 C 12.799 9.095,12.476 9.459,12.087 9.861 C 11.731 10.227,11.376 10.622,11.297 10.738 C 11.023 11.141,10.883 11.589,10.844 12.192 L 10.823 12.500 9.990 12.500 L 9.157 12.500 9.176 12.025 C 9.196 11.503,9.256 11.213,9.437 10.779 C 9.630 10.317,9.883 9.988,10.664 9.188 C 11.432 8.402,11.511 8.294,11.616 7.885 C 11.876 6.873,11.057 5.833,10.000 5.833 C 9.194 5.833,8.486 6.443,8.358 7.248 L 8.318 7.500 7.488 7.500 L 6.657 7.500 6.679 7.235 C 6.784 5.977,7.605 4.866,8.773 4.403 C 8.981 4.321,9.237 4.254,9.600 4.187 C 9.792 4.151,10.525 4.198,10.783 4.263 M10.833 15.000 L 10.833 15.833 10.000 15.833 L 9.167 15.833 9.167 15.000 L 9.167 14.167 10.000 14.167 L 10.833 14.167 10.833 15.000 " fillRule=`evenodd> </Path> </Svg>;
