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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M5.324 4.063 C 5.205 4.095,5.021 4.169,4.914 4.227 C 4.681 4.354,4.323 4.720,4.205 4.951 C 3.991 5.369,4.000 5.068,4.000 12.000 C 4.000 18.932,3.991 18.631,4.205 19.049 C 4.322 19.280,4.681 19.646,4.913 19.772 C 5.353 20.012,4.983 20.000,12.000 20.000 C 19.017 20.000,18.647 20.012,19.087 19.772 C 19.319 19.646,19.678 19.280,19.795 19.049 C 20.001 18.647,20.000 18.658,20.000 15.212 L 20.000 12.000 19.120 12.000 L 18.240 12.000 18.230 15.110 L 18.220 18.220 12.000 18.220 L 5.780 18.220 5.780 12.000 L 5.780 5.780 8.890 5.770 L 12.000 5.760 12.000 4.880 L 12.000 4.000 8.770 4.003 C 6.079 4.005,5.504 4.015,5.324 4.063 M13.760 4.880 L 13.760 5.760 15.340 5.760 C 16.209 5.760,16.920 5.773,16.920 5.790 C 16.920 5.806,14.963 7.777,12.570 10.170 L 8.220 14.520 8.850 15.150 L 9.480 15.780 13.830 11.430 C 16.223 9.037,18.194 7.080,18.210 7.080 C 18.227 7.080,18.240 7.791,18.240 8.660 L 18.240 10.240 19.120 10.240 L 20.000 10.240 20.000 7.120 L 20.000 4.000 16.880 4.000 L 13.760 4.000 13.760 4.880 " fillRule=`evenodd> </Path> </Svg>;
