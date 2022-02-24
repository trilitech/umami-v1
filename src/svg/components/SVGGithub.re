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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fill="none" fillRule=`evenodd> <Path d="M0 0h24v24H0z"/> <Path d="M2 11.74c0 4.304 2.865 7.955 6.839 9.243.5.09.682-.211.682-.47 0-.23-.008-.843-.013-1.656-2.782.588-3.369-1.306-3.369-1.306-.454-1.125-1.11-1.425-1.11-1.425-.908-.604.069-.592.069-.592 1.003.069 1.531 1.004 1.531 1.004.892 1.488 2.341 1.059 2.91.81.092-.63.35-1.06.636-1.303-2.22-.245-4.555-1.081-4.555-4.814 0-1.063.39-1.933 1.029-2.613-.103-.247-.446-1.238.098-2.578 0 0 .84-.262 2.75.998A9.818 9.818 0 0112 6.71c.85.004 1.705.112 2.504.328 1.909-1.26 2.747-.998 2.747-.998.546 1.34.203 2.331.1 2.578.64.68 1.028 1.55 1.028 2.613 0 3.742-2.339 4.566-4.566 4.807.359.3.678.895.678 1.804 0 1.301-.012 2.352-.012 2.671 0 .261.18.564.688.47C19.137 19.69 22 16.042 22 11.74 22 6.36 17.522 2 12 2S2 6.36 2 11.74z" fill="#161514"/> </G> </Svg>
;
