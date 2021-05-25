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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M17.000 6.520 L 17.000 8.000 13.500 8.000 L 10.000 8.000 10.000 9.000 L 10.000 10.000 13.500 10.000 L 17.000 10.000 17.000 11.480 C 17.000 12.294,17.014 12.960,17.031 12.960 C 17.047 12.960,17.952 12.069,19.040 10.980 L 21.019 8.999 19.040 7.020 C 17.951 5.931,17.047 5.040,17.030 5.040 C 17.014 5.040,17.000 5.706,17.000 6.520 M4.960 13.020 L 2.980 15.000 4.960 16.980 C 6.049 18.069,6.953 18.960,6.970 18.960 C 6.986 18.960,7.000 18.294,7.000 17.480 L 7.000 16.000 10.500 16.000 L 14.000 16.000 14.000 15.000 L 14.000 14.000 10.500 14.000 L 7.000 14.000 7.000 12.520 C 7.000 11.706,6.986 11.040,6.970 11.040 C 6.953 11.040,6.049 11.931,4.960 13.020 " fillRule=`evenodd> </Path> </Svg>;
