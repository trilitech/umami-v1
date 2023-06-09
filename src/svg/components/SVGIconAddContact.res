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
      d="M14.329 4.060 C 13.468 4.216,12.789 4.569,12.179 5.179 C 11.660 5.697,11.318 6.289,11.130 6.998 C 10.995 7.502,10.979 8.365,11.095 8.860 C 11.770 11.755,15.196 12.966,17.491 11.122 C 18.209 10.545,18.692 9.772,18.905 8.860 C 18.959 8.631,18.980 8.386,18.980 8.000 C 18.980 7.399,18.913 7.027,18.713 6.523 C 18.110 5.001,16.600 3.986,14.965 4.004 C 14.775 4.006,14.489 4.031,14.329 4.060 M15.462 6.061 C 15.848 6.160,16.117 6.314,16.398 6.597 C 17.198 7.401,17.199 8.601,16.400 9.400 C 15.976 9.824,15.447 10.029,14.880 9.989 C 13.869 9.917,13.088 9.135,13.012 8.120 C 12.970 7.552,13.174 7.023,13.599 6.598 C 14.104 6.093,14.802 5.892,15.462 6.061 M4.000 8.500 L 4.000 10.000 2.500 10.000 L 1.000 10.000 1.000 11.000 L 1.000 12.000 2.500 12.000 L 4.000 12.000 4.000 13.500 L 4.000 15.000 5.000 15.000 L 6.000 15.000 6.000 13.500 L 6.000 12.000 7.500 12.000 L 9.000 12.000 9.000 11.000 L 9.000 10.000 7.500 10.000 L 6.000 10.000 6.000 8.500 L 6.000 7.000 5.000 7.000 L 4.000 7.000 4.000 8.500 M14.040 14.046 C 12.676 14.173,10.986 14.586,9.820 15.078 C 8.391 15.681,7.433 16.482,7.112 17.342 C 7.029 17.565,7.023 17.658,7.009 18.790 L 6.995 20.000 15.000 20.000 L 23.005 20.000 22.991 18.790 C 22.977 17.658,22.971 17.565,22.888 17.342 C 22.557 16.456,21.529 15.617,20.046 15.022 C 18.144 14.259,15.818 13.881,14.040 14.046 M16.460 16.115 C 17.380 16.255,18.373 16.514,19.159 16.818 C 19.998 17.143,20.774 17.606,20.932 17.876 L 21.005 18.000 15.000 18.000 L 8.995 18.000 9.068 17.876 C 9.184 17.677,9.581 17.407,10.180 17.118 C 11.291 16.583,12.693 16.196,14.080 16.043 C 14.602 15.985,15.858 16.023,16.460 16.115 "
      fillRule=#evenodd
    />
  </Svg>
