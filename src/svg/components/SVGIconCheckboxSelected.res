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
      d="M4.534 3.057 C 3.808 3.233,3.196 3.863,3.044 4.592 C 2.982 4.892,2.982 19.108,3.044 19.408 C 3.201 20.158,3.842 20.799,4.592 20.956 C 4.892 21.018,19.108 21.018,19.408 20.956 C 20.158 20.799,20.799 20.158,20.956 19.408 C 21.018 19.108,21.018 4.892,20.956 4.592 C 20.799 3.842,20.158 3.201,19.408 3.044 C 19.085 2.977,4.814 2.989,4.534 3.057 M18.310 7.290 L 19.000 7.980 14.500 12.480 L 10.000 16.980 7.500 14.480 L 5.000 11.980 5.700 11.280 L 6.400 10.580 8.200 12.380 L 10.000 14.180 13.790 10.390 C 15.874 8.305,17.589 6.600,17.600 6.600 C 17.612 6.600,17.931 6.911,18.310 7.290 "
      fillRule=#evenodd
    />
  </Svg>
