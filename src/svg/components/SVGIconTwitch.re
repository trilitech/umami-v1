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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M4.091 2.150 L 2.002 4.300 2.001 12.010 L 2.000 19.720 4.500 19.720 L 6.999 19.720 7.010 21.843 L 7.020 23.967 9.080 21.844 L 11.140 19.721 12.808 19.720 L 14.477 19.720 18.238 15.851 L 22.000 11.983 22.000 5.991 L 22.000 0.000 14.090 0.000 L 6.180 0.001 4.091 2.150 M20.359 6.430 L 20.357 11.140 18.673 12.870 L 16.989 14.600 15.320 14.600 L 13.650 14.600 12.216 16.080 C 11.427 16.894,10.767 17.560,10.751 17.560 C 10.734 17.560,10.720 16.894,10.720 16.080 L 10.720 14.600 8.860 14.600 L 7.000 14.600 7.000 8.160 L 7.000 1.720 13.680 1.720 L 20.360 1.720 20.359 6.430 M11.600 7.280 L 11.600 9.840 12.420 9.840 L 13.240 9.840 13.240 7.280 L 13.240 4.720 12.420 4.720 L 11.600 4.720 11.600 7.280 M16.200 7.280 L 16.200 9.840 17.020 9.840 L 17.840 9.840 17.840 7.280 L 17.840 4.720 17.020 4.720 L 16.200 4.720 16.200 7.280 " fillRule=`evenodd> </Path> </Svg>;
