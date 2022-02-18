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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <G fill="none" fillRule=`evenodd> <Path d="M0 0h24v24H0z"/> <G fillRule=`nonzero> <Path fill="#E24329" d="M12 21l3.684-11.075H8.32z"/> <Path d="M3.16 9.926L2.036 13.29a.736.736 0 00.276.833L12 21 3.16 9.926z" fill="#FCA326"/> <Path d="M3.16 9.926h5.16L6.1 3.256c-.115-.341-.61-.341-.728 0L3.16 9.926z" fill="#E24329"/> <Path d="M20.845 9.926l1.118 3.364a.736.736 0 01-.276.833L11.999 21l8.846-11.074z" fill="#FCA326"/> <Path d="M20.845 9.926h-5.161l2.217-6.67c.114-.341.609-.341.727 0l2.217 6.67z" fill="#E24329"/> <Path fill="#FC6D26" d="M12 21l3.684-11.074h5.16zm0 0L3.16 9.926h5.16z"/> </G> </G> </Svg>
;
