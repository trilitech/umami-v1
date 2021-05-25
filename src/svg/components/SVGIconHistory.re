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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.240 3.045 C 8.180 3.398,5.608 5.184,4.235 7.909 C 3.667 9.039,3.281 10.508,3.280 11.547 C 3.280 11.726,3.269 11.902,3.256 11.937 C 3.235 11.992,3.113 12.000,2.306 12.000 C 1.458 12.000,1.369 12.007,1.252 12.078 C 1.031 12.213,0.938 12.504,1.051 12.706 C 1.078 12.754,1.744 13.450,2.531 14.254 C 4.016 15.769,4.103 15.842,4.352 15.781 C 4.472 15.752,7.325 12.938,7.423 12.752 C 7.454 12.693,7.480 12.571,7.480 12.481 C 7.480 12.352,7.452 12.288,7.343 12.169 L 7.206 12.020 6.217 12.008 L 5.228 11.996 5.252 11.548 C 5.350 9.737,6.232 7.928,7.599 6.729 C 9.653 4.930,12.493 4.501,14.911 5.623 C 16.136 6.192,17.211 7.142,17.940 8.300 C 19.079 10.111,19.331 12.290,18.635 14.310 C 17.564 17.417,14.471 19.360,11.266 18.940 C 10.133 18.791,9.210 18.430,8.189 17.736 C 7.952 17.574,7.682 17.422,7.589 17.397 C 7.256 17.307,6.888 17.429,6.632 17.712 C 6.244 18.141,6.309 18.797,6.775 19.170 C 7.337 19.618,8.412 20.210,9.158 20.481 C 12.636 21.746,16.637 20.619,18.975 17.717 C 21.859 14.134,21.633 9.062,18.440 5.744 C 16.737 3.973,14.426 2.982,12.060 3.005 C 11.774 3.008,11.405 3.026,11.240 3.045 M11.740 7.094 C 11.522 7.196,11.302 7.419,11.215 7.626 C 11.171 7.732,11.160 8.188,11.161 10.009 C 11.162 12.166,11.165 12.267,11.241 12.434 C 11.298 12.559,11.728 13.021,12.749 14.054 C 13.535 14.850,14.242 15.548,14.319 15.606 C 14.715 15.904,15.275 15.833,15.609 15.442 C 15.853 15.157,15.915 14.700,15.755 14.369 C 15.697 14.251,15.254 13.772,14.397 12.904 L 13.124 11.614 13.112 9.697 C 13.098 7.514,13.108 7.580,12.760 7.267 C 12.448 6.987,12.097 6.928,11.740 7.094 " fillRule=`evenodd> </Path> </Svg>;
