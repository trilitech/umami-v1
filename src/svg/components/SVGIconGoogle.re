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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.453 2.043 C 9.886 2.163,8.388 2.617,7.091 3.363 C 6.228 3.860,5.637 4.315,4.920 5.037 C 3.625 6.338,2.775 7.826,2.318 9.588 C 1.924 11.109,1.924 12.891,2.318 14.412 C 2.776 16.178,3.638 17.682,4.938 18.982 C 6.582 20.626,8.534 21.580,10.920 21.905 C 11.575 21.994,12.901 21.993,13.613 21.903 C 16.619 21.523,19.064 20.031,20.514 17.693 C 21.703 15.777,22.199 13.321,21.896 10.860 C 21.863 10.596,21.826 10.340,21.814 10.290 L 21.791 10.200 16.996 10.200 L 12.200 10.200 12.200 12.120 L 12.200 14.039 14.955 14.050 L 17.711 14.060 17.638 14.346 C 17.364 15.425,16.676 16.391,15.718 17.041 C 14.480 17.882,12.649 18.228,11.068 17.920 C 8.637 17.447,6.715 15.490,6.255 13.020 C 6.153 12.470,6.153 11.526,6.255 10.980 C 6.723 8.474,8.669 6.521,11.163 6.054 C 11.677 5.958,12.754 5.958,13.280 6.054 C 14.272 6.235,15.105 6.636,16.007 7.365 L 16.115 7.452 17.557 6.037 C 18.351 5.258,19.000 4.617,19.000 4.611 C 19.000 4.583,18.633 4.272,18.293 4.013 C 16.751 2.836,14.929 2.173,12.860 2.038 C 12.231 1.997,12.036 1.998,11.453 2.043 " fillRule=`evenodd> </Path> </Svg>;
