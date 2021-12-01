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
  <Svg viewBox="0 0 32 32" ?width ?height ?fill ?stroke>
    <Path
      d="M17.400 8.227 L 17.413 12.453 18.813 12.468 L 20.213 12.482 20.213 10.774 C 20.213 9.754,20.233 9.067,20.263 9.067 C 20.334 9.067,21.032 9.846,21.394 10.330 C 21.752 10.807,22.167 11.540,22.401 12.107 C 22.589 12.560,22.880 13.556,22.880 13.743 L 22.880 13.867 24.317 13.867 L 25.753 13.867 25.725 13.667 C 25.652 13.159,25.413 12.200,25.218 11.635 C 24.657 10.006,23.763 8.566,22.554 7.347 L 22.039 6.827 23.953 6.827 L 25.867 6.827 25.867 5.413 L 25.867 4.000 21.626 4.000 L 17.386 4.000 17.400 8.227 M11.093 4.582 C 8.110 5.589,5.841 7.576,4.478 10.373 C 3.913 11.532,3.552 12.765,3.410 14.021 C 3.327 14.750,3.373 16.561,3.490 17.190 C 3.934 19.567,4.946 21.516,6.632 23.240 L 7.167 23.787 5.263 23.787 L 3.360 23.787 3.360 25.173 L 3.360 26.560 7.573 26.560 L 11.787 26.560 11.787 22.347 L 11.787 18.133 10.373 18.133 L 8.960 18.133 8.958 19.853 L 8.956 21.573 8.543 21.142 C 5.989 18.477,5.422 14.656,7.099 11.411 C 7.904 9.853,9.212 8.557,10.773 7.768 C 11.140 7.582,11.518 7.404,11.613 7.372 L 11.787 7.312 11.787 5.843 C 11.787 5.035,11.769 4.374,11.747 4.374 C 11.725 4.375,11.431 4.468,11.093 4.582 M17.374 16.762 C 17.278 16.805,16.781 17.428,15.921 18.585 C 14.460 20.549,14.472 20.526,14.787 20.890 C 15.304 21.489,21.205 27.779,21.344 27.880 C 21.553 28.032,21.700 28.032,21.912 27.880 C 22.004 27.814,23.556 26.178,25.360 24.244 C 28.476 20.905,28.640 20.719,28.640 20.531 C 28.640 20.356,28.490 20.132,27.334 18.582 C 26.615 17.619,25.973 16.801,25.907 16.764 C 25.736 16.669,17.590 16.668,17.374 16.762 "
      fillRule=`evenodd
    />
  </Svg>;
