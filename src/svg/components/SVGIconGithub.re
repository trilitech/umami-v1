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
    ) => <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke> <Path d="M11.240 2.040 C 8.068 2.293,5.267 3.940,3.552 6.560 C 2.202 8.623,1.713 11.180,2.204 13.620 C 2.799 16.576,4.793 19.114,7.580 20.460 C 8.552 20.929,9.060 21.061,9.299 20.905 C 9.510 20.766,9.534 20.628,9.503 19.721 C 9.488 19.271,9.471 18.898,9.466 18.892 C 9.460 18.887,9.339 18.902,9.198 18.926 C 8.770 18.998,8.065 18.978,7.708 18.884 C 6.940 18.682,6.462 18.270,6.096 17.491 C 5.758 16.770,5.525 16.479,4.987 16.103 C 4.581 15.819,4.535 15.647,4.842 15.555 C 4.967 15.517,5.069 15.516,5.256 15.548 C 5.788 15.639,6.265 15.984,6.652 16.558 C 7.049 17.148,7.439 17.428,8.035 17.549 C 8.387 17.621,8.785 17.586,9.250 17.441 C 9.481 17.369,9.522 17.342,9.542 17.249 C 9.635 16.811,9.767 16.519,10.029 16.170 L 10.117 16.052 9.769 16.004 C 7.987 15.758,6.753 15.006,6.134 13.786 C 5.781 13.091,5.627 12.339,5.627 11.320 C 5.628 10.733,5.641 10.570,5.712 10.310 C 5.877 9.705,6.084 9.292,6.451 8.835 C 6.601 8.648,6.634 8.583,6.604 8.527 C 6.583 8.488,6.535 8.305,6.498 8.120 C 6.381 7.546,6.467 6.690,6.697 6.141 L 6.761 5.988 7.082 6.013 C 7.638 6.056,8.448 6.371,9.199 6.837 C 9.459 6.998,9.519 7.020,9.621 6.992 C 9.983 6.892,10.881 6.755,11.393 6.723 C 12.315 6.665,13.627 6.783,14.377 6.992 C 14.476 7.019,14.542 6.995,14.817 6.829 C 15.614 6.346,16.363 6.056,16.918 6.013 L 17.239 5.988 17.303 6.141 C 17.531 6.686,17.618 7.550,17.503 8.119 C 17.465 8.304,17.417 8.488,17.396 8.527 C 17.366 8.583,17.401 8.650,17.562 8.849 C 18.300 9.757,18.540 10.913,18.299 12.387 C 18.102 13.593,17.582 14.499,16.739 15.104 C 16.134 15.538,15.075 15.902,14.070 16.022 L 13.839 16.049 13.978 16.203 C 14.160 16.405,14.344 16.773,14.430 17.110 C 14.490 17.344,14.500 17.597,14.500 19.022 C 14.500 20.863,14.494 20.822,14.793 20.949 C 14.927 21.006,14.963 21.006,15.191 20.947 C 15.823 20.783,16.885 20.269,17.658 19.752 C 18.413 19.247,19.314 18.425,19.862 17.740 C 20.964 16.363,21.652 14.759,21.903 12.980 C 21.984 12.409,21.995 11.160,21.924 10.620 C 21.636 8.431,20.729 6.568,19.193 5.009 C 17.634 3.428,15.674 2.441,13.460 2.124 C 12.801 2.029,11.830 1.993,11.240 2.040 " fillRule=`evenodd> </Path> </Svg>;
