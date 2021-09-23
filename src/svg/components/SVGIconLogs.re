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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M3.520 2.064 C 2.780 2.260,2.215 2.852,2.058 3.595 C 2.010 3.818,2.001 5.298,2.001 12.921 L 2.000 21.982 3.989 19.991 L 5.979 18.000 13.101 18.000 C 20.892 18.000,20.422 18.014,20.920 17.770 C 21.201 17.632,21.627 17.205,21.769 16.919 C 22.013 16.427,22.000 16.807,22.000 10.000 C 22.000 3.206,22.012 3.578,21.773 3.084 C 21.634 2.797,21.201 2.363,20.920 2.228 C 20.414 1.985,20.994 2.000,11.982 2.003 C 4.886 2.005,3.709 2.013,3.520 2.064 M20.000 10.000 L 20.000 16.000 12.590 16.000 L 5.181 16.000 4.590 16.589 L 4.000 17.179 4.000 10.589 L 4.000 4.000 12.000 4.000 L 20.000 4.000 20.000 10.000 M11.000 8.000 L 11.000 10.000 12.000 10.000 L 13.000 10.000 13.000 8.000 L 13.000 6.000 12.000 6.000 L 11.000 6.000 11.000 8.000 M11.000 13.000 L 11.000 14.000 12.000 14.000 L 13.000 14.000 13.000 13.000 L 13.000 12.000 12.000 12.000 L 11.000 12.000 11.000 13.000 " fillRule=`evenodd> </Path> </Svg>;
