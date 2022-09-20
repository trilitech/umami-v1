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
      d="M11.140 4.543 C 11.096 4.549,10.862 4.575,10.620 4.601 C 7.902 4.889,5.163 6.290,3.309 8.340 C 2.836 8.864,2.307 9.556,1.980 10.080 C 1.713 10.508,1.168 11.573,1.082 11.833 C 1.011 12.048,1.027 12.096,1.423 12.900 C 3.029 16.159,6.053 18.503,9.601 19.238 C 10.290 19.381,10.821 19.440,11.640 19.466 C 13.644 19.530,15.437 19.141,17.224 18.258 C 19.036 17.362,20.552 16.049,21.728 14.358 C 22.224 13.645,22.960 12.236,22.960 12.000 C 22.960 11.876,22.568 11.030,22.260 10.491 C 21.101 8.462,19.389 6.835,17.301 5.778 C 15.616 4.926,13.838 4.511,11.931 4.525 C 11.540 4.528,11.184 4.536,11.140 4.543 M12.825 7.061 C 14.450 7.342,15.768 8.330,16.505 9.820 C 17.271 11.369,17.146 13.293,16.185 14.747 C 14.658 17.057,11.552 17.702,9.253 16.186 C 7.835 15.250,7.000 13.700,7.000 12.000 C 7.000 9.558,8.729 7.497,11.140 7.065 C 11.572 6.987,12.386 6.986,12.825 7.061 M11.680 9.026 C 10.399 9.195,9.390 10.086,9.093 11.311 C 9.007 11.664,9.007 12.336,9.093 12.689 C 9.358 13.780,10.220 14.642,11.311 14.907 C 11.667 14.993,12.335 14.992,12.700 14.905 C 13.766 14.650,14.645 13.767,14.907 12.689 C 14.993 12.336,14.993 11.664,14.907 11.311 C 14.700 10.459,14.118 9.724,13.348 9.344 C 13.146 9.244,12.863 9.135,12.720 9.103 C 12.433 9.037,11.897 8.998,11.680 9.026 "
      fillRule=#evenodd
    />
  </Svg>
