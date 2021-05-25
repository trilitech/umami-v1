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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M3.337 1.081 C 3.235 1.131,3.141 1.220,3.087 1.316 L 3.000 1.471 3.000 12.000 L 3.000 22.529 3.085 22.680 C 3.131 22.763,3.237 22.869,3.320 22.915 L 3.471 23.000 11.265 22.999 C 17.881 22.999,19.101 22.990,19.331 22.941 C 19.931 22.814,20.489 22.378,20.761 21.825 C 20.849 21.646,20.938 21.403,20.960 21.285 C 21.015 20.989,21.015 3.011,20.960 2.715 C 20.817 1.938,20.115 1.225,19.331 1.059 C 19.101 1.010,17.883 1.001,11.280 1.001 C 3.557 1.000,3.499 1.001,3.337 1.081 M6.000 12.000 L 6.000 21.000 5.500 21.000 L 5.000 21.000 5.000 12.000 L 5.000 3.000 5.500 3.000 L 6.000 3.000 6.000 12.000 M18.250 3.114 C 18.509 3.241,18.784 3.519,18.902 3.771 L 18.999 3.980 18.999 12.000 L 18.999 20.020 18.906 20.221 C 18.790 20.470,18.502 20.762,18.251 20.886 L 18.060 20.980 12.530 20.990 L 7.000 21.001 7.000 12.000 L 7.000 2.999 12.530 3.010 L 18.060 3.020 18.250 3.114 M12.540 8.574 C 11.892 8.788,11.432 9.320,11.315 9.990 C 11.192 10.695,11.601 11.478,12.264 11.805 C 13.330 12.330,14.572 11.685,14.768 10.505 C 14.889 9.774,14.496 9.030,13.810 8.691 C 13.528 8.552,13.479 8.541,13.104 8.531 C 12.849 8.525,12.641 8.541,12.540 8.574 M12.460 12.905 C 11.183 13.098,10.176 13.497,9.783 13.963 C 9.543 14.249,9.508 14.375,9.504 14.960 L 9.500 15.500 13.000 15.500 L 16.500 15.500 16.497 14.958 C 16.494 14.439,16.488 14.406,16.378 14.204 C 16.162 13.807,15.716 13.503,14.960 13.237 C 14.351 13.022,13.768 12.915,13.120 12.900 C 12.823 12.893,12.526 12.895,12.460 12.905 " fillRule=`evenodd> </Path> </Svg>;
