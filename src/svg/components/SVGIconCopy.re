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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.203 3.064 C 5.084 3.096,4.922 3.160,4.843 3.207 C 4.639 3.329,4.333 3.632,4.219 3.826 C 3.991 4.214,4.000 3.952,4.000 10.268 L 4.000 16.080 4.820 16.080 L 5.640 16.080 5.640 10.360 L 5.640 4.640 10.540 4.640 L 15.440 4.640 15.440 3.820 L 15.440 3.000 10.430 3.003 C 6.174 3.005,5.387 3.014,5.203 3.064 M8.510 6.323 C 8.067 6.428,7.637 6.788,7.428 7.230 L 7.300 7.500 7.300 13.641 L 7.300 19.782 7.432 20.050 C 7.576 20.342,7.834 20.626,8.094 20.778 C 8.483 21.007,8.325 21.000,13.418 21.000 C 17.256 20.999,18.137 20.989,18.306 20.944 C 18.949 20.773,19.414 20.257,19.518 19.600 C 19.572 19.259,19.575 7.996,19.521 7.667 C 19.418 7.035,18.971 6.525,18.363 6.344 C 18.168 6.287,17.652 6.280,13.403 6.284 C 10.794 6.286,8.592 6.304,8.510 6.323 M17.920 13.640 L 17.920 19.360 13.420 19.360 L 8.920 19.360 8.920 13.640 L 8.920 7.920 13.420 7.920 L 17.920 7.920 17.920 13.640 " fillRule=`evenodd> </Path> </Svg>;
