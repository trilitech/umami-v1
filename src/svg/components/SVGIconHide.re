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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M2.710 3.150 C 2.341 3.519,2.040 3.839,2.040 3.860 C 2.040 3.882,2.635 4.495,3.363 5.223 L 4.687 6.547 4.473 6.724 C 3.044 7.907,1.867 9.475,1.140 11.163 L 0.993 11.506 1.223 12.017 C 2.704 15.306,5.643 17.770,9.120 18.639 C 10.188 18.906,10.823 18.978,12.060 18.976 C 12.881 18.974,13.164 18.958,13.640 18.886 C 14.391 18.772,15.084 18.605,15.773 18.373 L 16.326 18.187 18.033 19.893 L 19.741 21.600 20.430 20.910 C 20.810 20.530,21.120 20.201,21.120 20.180 C 21.120 20.127,3.472 2.480,3.420 2.480 C 3.397 2.480,3.078 2.782,2.710 3.150 M11.240 4.026 C 10.481 4.098,10.149 4.141,9.724 4.225 C 9.279 4.313,8.412 4.535,8.377 4.570 C 8.369 4.577,8.730 4.951,9.179 5.399 L 9.995 6.215 10.428 6.146 C 11.738 5.937,12.721 5.953,13.973 6.203 C 15.419 6.491,16.950 7.199,18.100 8.112 C 18.493 8.424,19.195 9.114,19.552 9.540 C 19.914 9.971,20.428 10.739,20.649 11.178 L 20.809 11.497 20.661 11.798 C 20.301 12.526,19.696 13.345,18.972 14.080 C 18.680 14.377,18.440 14.629,18.440 14.640 C 18.440 14.650,18.750 14.969,19.128 15.348 L 19.816 16.037 20.078 15.789 C 21.287 14.640,22.234 13.295,22.860 11.837 L 23.007 11.494 22.777 10.983 C 21.870 8.968,20.384 7.214,18.540 5.981 C 16.591 4.677,14.260 3.982,11.920 4.007 C 11.623 4.010,11.317 4.019,11.240 4.026 M11.302 7.065 C 11.128 7.096,10.976 7.131,10.964 7.142 C 10.953 7.154,11.393 7.613,11.942 8.163 C 12.716 8.939,12.998 9.195,13.200 9.308 C 13.533 9.494,13.986 9.947,14.172 10.280 C 14.285 10.482,14.544 10.767,15.339 11.560 C 15.900 12.121,16.360 12.571,16.360 12.560 C 16.360 12.549,16.387 12.387,16.420 12.200 C 16.575 11.320,16.421 10.271,16.020 9.480 C 15.141 7.744,13.159 6.729,11.302 7.065 M6.998 8.858 L 7.856 9.717 7.770 9.968 C 7.591 10.489,7.554 10.700,7.532 11.320 C 7.507 12.040,7.562 12.418,7.778 13.023 C 7.941 13.478,8.127 13.818,8.443 14.238 C 9.576 15.742,11.656 16.376,13.435 15.761 L 13.780 15.642 14.258 16.119 C 14.521 16.382,14.726 16.607,14.714 16.619 C 14.702 16.632,14.495 16.685,14.256 16.739 C 12.531 17.123,10.856 17.075,9.200 16.594 C 8.030 16.254,6.804 15.611,5.823 14.822 C 5.345 14.436,4.531 13.597,4.170 13.114 C 3.857 12.697,3.437 12.023,3.285 11.694 L 3.191 11.490 3.365 11.155 C 3.581 10.740,4.028 10.067,4.363 9.653 C 4.814 9.097,5.964 8.003,6.100 8.001 C 6.123 8.000,6.527 8.386,6.998 8.858 M12.080 13.976 C 12.080 14.035,11.522 13.970,11.240 13.878 C 10.843 13.748,10.569 13.575,10.238 13.243 C 9.771 12.774,9.520 12.212,9.520 11.634 L 9.520 11.380 10.800 12.660 C 11.504 13.364,12.080 13.956,12.080 13.976 " fillRule=`evenodd> </Path> </Svg>;
