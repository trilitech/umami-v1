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

open ReactNative
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
      d="M12.000 2.482 L 12.000 3.983 11.390 4.033 C 9.390 4.195,7.643 5.017,6.259 6.446 C 5.611 7.114,5.233 7.653,4.817 8.500 C 3.825 10.518,3.739 12.861,4.580 14.962 C 4.750 15.385,5.191 16.240,5.240 16.240 C 5.251 16.240,5.583 15.917,5.977 15.523 L 6.694 14.806 6.544 14.479 C 6.342 14.037,6.185 13.540,6.095 13.060 C 5.984 12.470,5.994 11.410,6.115 10.840 C 6.691 8.137,8.890 6.206,11.610 6.015 L 12.000 5.988 12.000 7.493 L 12.000 8.997 13.930 7.064 C 14.992 6.000,15.891 5.096,15.929 5.054 C 15.995 4.980,15.928 4.908,13.999 2.979 L 12.000 0.980 12.000 2.482 M18.024 8.476 L 17.308 9.193 17.471 9.566 C 17.856 10.454,17.977 11.039,17.976 12.020 C 17.975 12.613,17.959 12.814,17.885 13.160 C 17.307 15.865,15.110 17.794,12.390 17.985 L 12.000 18.012 12.000 16.496 L 12.000 14.980 9.997 16.983 L 7.995 18.985 9.998 20.983 L 12.001 22.980 12.000 21.498 L 12.000 20.017 12.610 19.967 C 13.664 19.882,14.550 19.644,15.471 19.200 C 16.293 18.804,16.885 18.389,17.554 17.741 C 18.266 17.052,18.744 16.395,19.183 15.500 C 20.175 13.484,20.261 11.138,19.420 9.038 C 19.250 8.615,18.809 7.760,18.760 7.760 C 18.749 7.760,18.417 8.082,18.024 8.476 "
      fillRule=#evenodd
    />
  </Svg>
