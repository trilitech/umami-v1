open ReactNative
/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open ReactNativeSvg

@react.component
let make = (
  ~width: option<Style.size>=?,
  ~height: option<Style.size>=?,
  ~fill: option<string>=?,
  ~stroke: option<string>=?,
) =>
  <Svg viewBox="0 0 24 24" ?width ?height ?fill ?stroke>
    <Path
      d="M11.260 2.023 C 11.194 2.032,10.969 2.059,10.760 2.083 C 9.544 2.221,8.182 2.671,7.091 3.294 C 4.825 4.588,3.174 6.650,2.427 9.120 C 2.131 10.102,2.021 10.880,2.021 12.000 C 2.021 13.120,2.131 13.898,2.427 14.880 C 3.375 18.015,5.773 20.469,8.900 21.501 C 9.953 21.849,10.809 21.980,12.020 21.978 C 13.655 21.975,14.974 21.674,16.380 20.980 C 19.438 19.473,21.449 16.666,21.926 13.240 C 21.995 12.744,21.995 11.256,21.926 10.760 C 21.694 9.093,21.115 7.595,20.194 6.280 C 18.618 4.029,16.212 2.532,13.500 2.115 C 13.007 2.039,11.569 1.980,11.260 2.023 M12.766 4.041 C 14.260 4.189,15.700 4.758,16.900 5.674 C 17.260 5.949,17.933 6.616,18.239 7.000 C 19.543 8.638,20.162 10.713,19.959 12.766 C 19.770 14.678,18.935 16.403,17.554 17.741 C 16.886 18.389,16.347 18.767,15.500 19.183 C 12.392 20.712,8.665 20.048,6.238 17.534 C 5.616 16.889,5.223 16.327,4.817 15.500 C 3.481 12.783,3.813 9.535,5.675 7.100 C 5.926 6.771,6.518 6.163,6.880 5.860 C 8.510 4.500,10.667 3.833,12.766 4.041 M13.290 10.890 L 10.000 14.180 8.200 12.380 L 6.400 10.580 5.698 11.283 L 4.996 11.985 7.498 14.488 L 10.000 16.991 14.002 12.988 L 18.004 8.985 17.313 8.293 C 16.932 7.912,16.612 7.600,16.600 7.600 C 16.589 7.600,15.099 9.080,13.290 10.890 "
      fillRule=#evenodd
    />
  </Svg>
