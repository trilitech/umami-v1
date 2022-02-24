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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fill="none" fillRule=`evenodd> <Path d="M0 0h24v24H0z"/> <Path d="M22 12c0-5.523-4.477-10-10-10C6.475 2 1.998 6.476 1.998 12c0 4.991 3.657 9.128 8.438 9.879V14.89h-2.54V12h2.54V9.796c0-2.506 1.493-3.89 3.777-3.89 1.094 0 2.239.195 2.239.195v2.461h-1.261c-1.243 0-1.63.77-1.63 1.562V12h2.774l-.444 2.89h-2.33v6.989c4.78-.75 8.438-4.888 8.438-9.88" fill="#1877F2"/> <Path d="M15.892 14.89l.444-2.89h-2.774v-1.876c0-.791.387-1.562 1.63-1.562h1.26V6.101s-1.144-.195-2.238-.195c-2.284 0-3.777 1.384-3.777 3.89V12h-2.54v2.89h2.54v6.989a10.076 10.076 0 003.125 0V14.89h2.33" fill="#FFF"/> </G> </Svg>
;
