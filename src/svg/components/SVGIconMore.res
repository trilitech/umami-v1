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
      d="M11.520 4.064 C 11.148 4.162,10.877 4.320,10.597 4.601 C 9.723 5.474,9.828 6.851,10.826 7.604 C 11.420 8.053,12.215 8.116,12.920 7.770 C 13.201 7.632,13.627 7.205,13.769 6.919 C 14.171 6.107,14.034 5.238,13.404 4.600 C 12.901 4.092,12.184 3.887,11.520 4.064 M11.520 10.064 C 11.148 10.162,10.877 10.320,10.597 10.601 C 9.723 11.474,9.828 12.851,10.826 13.604 C 11.420 14.053,12.215 14.116,12.920 13.770 C 13.201 13.632,13.627 13.205,13.769 12.919 C 14.171 12.107,14.034 11.238,13.404 10.600 C 12.901 10.092,12.184 9.887,11.520 10.064 M11.520 16.064 C 11.148 16.162,10.877 16.320,10.597 16.601 C 9.723 17.474,9.828 18.851,10.826 19.604 C 11.420 20.053,12.215 20.116,12.920 19.770 C 13.201 19.632,13.627 19.205,13.769 18.919 C 14.171 18.107,14.034 17.238,13.404 16.600 C 12.901 16.092,12.184 15.887,11.520 16.064 "
      fillRule=#evenodd
    />
  </Svg>
