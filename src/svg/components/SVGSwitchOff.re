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
    ) =>
  <Svg viewBox="0 0 34 24" ?width ?height ?fill ?stroke>
    <Path
      d="M5.678 6.327 C 3.540 6.621,1.759 8.092,1.046 10.153 C 0.812 10.827,0.754 11.195,0.754 12.000 C 0.754 12.805,0.812 13.173,1.046 13.847 C 1.659 15.619,3.055 16.962,4.826 17.486 C 5.628 17.723,5.377 17.715,12.244 17.715 C 19.301 17.715,18.856 17.732,19.756 17.431 C 21.707 16.778,23.096 15.209,23.559 13.134 C 23.689 12.552,23.691 11.445,23.562 10.861 C 23.111 8.816,21.692 7.217,19.755 6.569 C 18.866 6.271,19.357 6.291,12.366 6.281 C 7.245 6.273,6.007 6.282,5.678 6.327 M6.997 8.585 C 8.454 8.815,9.609 9.954,9.863 11.411 C 9.942 11.859,9.942 12.140,9.864 12.588 C 9.675 13.670,8.978 14.603,7.985 15.106 C 7.869 15.164,7.613 15.261,7.416 15.321 C 7.105 15.415,6.982 15.430,6.488 15.433 C 5.803 15.437,5.534 15.378,4.942 15.091 C 4.250 14.755,3.751 14.269,3.399 13.587 C 3.148 13.100,3.049 12.726,3.022 12.160 C 2.996 11.615,3.066 11.185,3.257 10.705 C 3.847 9.228,5.420 8.336,6.997 8.585 "
      fillRule=`evenodd
    />
  </Svg>;
