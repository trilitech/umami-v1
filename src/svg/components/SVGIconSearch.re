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
      d="M8.860 3.044 C 6.062 3.336,3.757 5.393,3.165 8.126 C 2.858 9.542,3.022 10.972,3.640 12.270 C 4.501 14.079,6.185 15.422,8.117 15.841 C 9.953 16.239,11.893 15.810,13.402 14.673 L 13.726 14.430 13.863 14.563 L 14.000 14.696 14.000 15.099 L 14.000 15.502 16.190 17.685 C 17.626 19.115,18.435 19.893,18.540 19.943 C 18.770 20.054,19.242 20.048,19.463 19.932 C 19.972 19.663,20.175 19.027,19.910 18.533 C 19.856 18.433,18.944 17.483,17.655 16.183 L 15.489 14.000 15.093 14.000 L 14.696 14.000 14.563 13.863 L 14.430 13.726 14.673 13.402 C 15.956 11.700,16.326 9.474,15.662 7.456 C 15.509 6.990,15.078 6.153,14.770 5.722 C 14.425 5.238,13.762 4.575,13.278 4.230 C 12.852 3.926,12.012 3.492,11.560 3.343 C 10.702 3.061,9.729 2.953,8.860 3.044 M10.574 5.115 C 11.433 5.344,12.060 5.703,12.678 6.322 C 13.295 6.938,13.647 7.551,13.888 8.426 C 13.961 8.693,13.974 8.850,13.976 9.480 C 13.979 10.302,13.933 10.560,13.667 11.221 C 13.241 12.284,12.283 13.241,11.220 13.668 C 10.548 13.937,10.340 13.975,9.500 13.975 C 8.660 13.975,8.452 13.937,7.780 13.668 C 6.721 13.243,5.757 12.279,5.332 11.220 C 5.063 10.548,5.025 10.340,5.025 9.500 C 5.025 8.660,5.063 8.452,5.332 7.780 C 5.886 6.401,7.193 5.342,8.693 5.057 C 9.125 4.975,10.168 5.007,10.574 5.115 "
      fillRule=`evenodd
    />
  </Svg>;
