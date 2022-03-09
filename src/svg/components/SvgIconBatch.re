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
  <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke>
    <Path
      d="M2.687 2.570 C 2.480 2.637,2.177 2.934,2.093 3.151 C 1.896 3.664,2.116 4.254,2.591 4.486 C 2.757 4.567,2.859 4.581,3.419 4.599 L 4.058 4.620 5.886 8.599 L 7.714 12.578 7.581 12.819 C 6.945 13.975,6.266 15.284,6.199 15.484 C 5.814 16.625,6.480 17.881,7.628 18.180 C 7.826 18.232,8.646 18.240,13.678 18.240 L 19.500 18.239 19.700 18.146 C 20.229 17.900,20.445 17.278,20.185 16.746 C 20.124 16.623,20.009 16.467,19.927 16.400 C 19.623 16.148,19.935 16.160,13.753 16.160 C 10.655 16.160,8.120 16.147,8.120 16.132 C 8.120 16.117,8.367 15.640,8.669 15.072 L 9.218 14.040 13.107 14.040 C 15.792 14.040,17.062 14.026,17.208 13.996 C 17.674 13.899,18.174 13.569,18.436 13.185 C 18.498 13.094,19.179 11.787,19.949 10.280 C 22.135 6.004,21.974 6.339,21.976 6.060 C 21.977 5.752,21.906 5.549,21.728 5.352 C 21.509 5.110,21.311 5.022,20.980 5.022 C 20.652 5.022,20.434 5.117,20.254 5.337 C 20.191 5.413,19.393 6.935,18.480 8.717 L 16.820 11.959 13.236 11.959 L 9.652 11.960 7.492 7.250 L 5.332 2.540 4.076 2.534 C 3.275 2.530,2.772 2.543,2.687 2.570 M9.180 18.775 C 8.905 18.845,8.740 18.938,8.545 19.133 C 8.294 19.385,8.190 19.635,8.169 20.040 C 8.155 20.319,8.168 20.421,8.238 20.608 C 8.351 20.907,8.624 21.206,8.912 21.348 C 9.111 21.446,9.186 21.460,9.500 21.460 C 9.816 21.459,9.886 21.446,10.071 21.348 C 10.935 20.895,11.098 19.761,10.394 19.092 C 10.100 18.812,9.573 18.674,9.180 18.775 M16.734 18.760 C 16.202 18.870,15.795 19.294,15.681 19.853 C 15.552 20.486,15.932 21.168,16.542 21.398 C 16.835 21.509,17.240 21.497,17.527 21.370 C 17.936 21.189,18.278 20.730,18.340 20.280 C 18.371 20.057,18.305 19.687,18.199 19.480 C 17.929 18.955,17.295 18.643,16.734 18.760 "
      fillRule=`evenodd
    />
  </Svg>;
