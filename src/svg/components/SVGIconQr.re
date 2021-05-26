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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M3.000 6.000 L 3.000 9.000 6.000 9.000 L 9.000 9.000 9.000 6.000 L 9.000 3.000 6.000 3.000 L 3.000 3.000 3.000 6.000 M12.000 4.000 L 12.000 5.000 11.000 5.000 L 10.000 5.000 10.000 7.000 L 10.000 9.000 12.000 9.000 L 14.000 9.000 14.000 8.000 L 14.000 7.000 13.000 7.000 L 12.000 7.000 12.000 6.000 L 12.000 5.000 13.000 5.000 L 14.000 5.000 14.000 4.000 L 14.000 3.000 13.000 3.000 L 12.000 3.000 12.000 4.000 M15.000 6.000 L 15.000 9.000 18.000 9.000 L 21.000 9.000 21.000 6.000 L 21.000 3.000 18.000 3.000 L 15.000 3.000 15.000 6.000 M7.480 6.000 L 7.480 7.480 6.000 7.480 L 4.520 7.480 4.520 6.000 L 4.520 4.520 6.000 4.520 L 7.480 4.520 7.480 6.000 M19.480 6.000 L 19.480 7.480 18.000 7.480 L 16.520 7.480 16.520 6.000 L 16.520 4.520 18.000 4.520 L 19.480 4.520 19.480 6.000 M5.240 6.000 L 5.240 6.760 6.000 6.760 L 6.760 6.760 6.760 6.000 L 6.760 5.240 6.000 5.240 L 5.240 5.240 5.240 6.000 M17.240 6.000 L 17.240 6.760 18.000 6.760 L 18.760 6.760 18.760 6.000 L 18.760 5.240 18.000 5.240 L 17.240 5.240 17.240 6.000 M3.000 11.000 L 3.000 12.000 4.000 12.000 L 5.000 12.000 5.000 13.000 L 5.000 14.000 6.000 14.000 L 7.000 14.000 7.000 13.000 L 7.000 12.000 8.000 12.000 L 9.000 12.000 9.000 13.000 L 9.000 14.000 10.000 14.000 L 11.000 14.000 11.000 16.500 L 11.000 19.000 12.000 19.000 L 13.000 19.000 13.000 20.000 L 13.000 21.000 14.000 21.000 L 15.000 21.000 15.000 20.000 L 15.000 19.000 14.000 19.000 L 13.000 19.000 13.000 16.500 L 13.000 14.000 15.000 14.000 L 17.000 14.000 17.000 13.000 L 17.000 12.000 18.000 12.000 L 19.000 12.000 19.000 14.000 L 19.000 16.000 17.000 16.000 L 15.000 16.000 15.000 17.000 L 15.000 18.000 16.000 18.000 L 17.000 18.000 17.000 19.500 L 17.000 21.000 19.000 21.000 L 21.000 21.000 21.000 20.000 L 21.000 19.000 20.000 19.000 L 19.000 19.000 19.000 17.500 L 19.000 16.000 20.000 16.000 L 21.000 16.000 21.000 14.000 L 21.000 12.000 20.000 12.000 L 19.000 12.000 19.000 11.000 L 19.000 10.000 18.000 10.000 L 17.000 10.000 17.000 11.000 L 17.000 12.000 15.000 12.000 L 13.000 12.000 13.000 11.000 L 13.000 10.000 10.000 10.000 L 7.000 10.000 7.000 11.000 L 7.000 12.000 6.000 12.000 L 5.000 12.000 5.000 11.000 L 5.000 10.000 4.000 10.000 L 3.000 10.000 3.000 11.000 M13.000 13.000 L 13.000 14.000 12.000 14.000 L 11.000 14.000 11.000 13.000 L 11.000 12.000 12.000 12.000 L 13.000 12.000 13.000 13.000 M3.000 18.000 L 3.000 21.000 6.000 21.000 L 9.000 21.000 9.000 18.000 L 9.000 15.000 6.000 15.000 L 3.000 15.000 3.000 18.000 M7.480 18.000 L 7.480 19.480 6.000 19.480 L 4.520 19.480 4.520 18.000 L 4.520 16.520 6.000 16.520 L 7.480 16.520 7.480 18.000 M5.240 18.000 L 5.240 18.760 6.000 18.760 L 6.760 18.760 6.760 18.000 L 6.760 17.240 6.000 17.240 L 5.240 17.240 5.240 18.000 " fillRule=`evenodd> </Path> </Svg>;
