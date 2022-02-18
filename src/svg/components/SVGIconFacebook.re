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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.460 2.024 C 9.992 2.137,8.789 2.454,7.600 3.043 C 5.189 4.239,3.411 6.260,2.544 8.793 C 2.179 9.859,2.027 10.801,2.027 12.000 C 2.027 13.609,2.347 14.995,3.041 16.394 C 4.040 18.409,5.574 19.950,7.564 20.938 C 8.418 21.361,9.543 21.735,10.230 21.823 L 10.440 21.850 10.440 18.385 L 10.440 14.920 9.160 14.920 L 7.880 14.920 7.880 13.460 L 7.880 12.000 9.157 12.000 L 10.434 12.000 10.450 10.450 C 10.465 9.032,10.474 8.873,10.551 8.580 C 10.967 7.001,12.038 6.097,13.708 5.917 C 14.272 5.856,15.176 5.903,16.170 6.045 L 16.480 6.089 16.480 7.325 L 16.480 8.560 15.890 8.561 C 14.609 8.562,14.127 8.710,13.823 9.197 C 13.609 9.539,13.590 9.662,13.570 10.870 L 13.551 12.000 14.956 12.000 L 16.362 12.000 16.341 12.090 C 16.329 12.139,16.230 12.774,16.119 13.500 C 16.009 14.226,15.910 14.843,15.901 14.870 C 15.887 14.908,15.614 14.920,14.721 14.920 L 13.560 14.920 13.560 18.380 L 13.560 21.840 13.662 21.840 C 13.850 21.840,14.648 21.641,15.195 21.458 C 17.916 20.550,20.136 18.458,21.255 15.749 C 21.999 13.951,22.178 11.733,21.738 9.783 C 21.418 8.366,20.685 6.863,19.748 5.700 C 18.189 3.766,15.793 2.425,13.320 2.104 C 12.843 2.042,11.809 1.998,11.460 2.024 " fillRule=`evenodd> </Path> </Svg>;
