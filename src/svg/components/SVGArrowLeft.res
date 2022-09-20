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
  <Svg viewBox="0 0 36 36" ?width ?height ?fill ?stroke>
    <Path
      d="M17.640 6.653 C 17.557 6.680,17.395 6.749,17.280 6.807 C 17.164 6.865,14.783 9.198,11.987 11.991 C 6.478 17.497,6.643 17.312,6.643 17.971 C 6.643 18.630,6.384 18.339,11.962 23.926 C 15.608 27.579,17.205 29.140,17.386 29.227 C 17.705 29.382,18.230 29.392,18.585 29.249 C 18.901 29.122,19.333 28.654,19.429 28.334 C 19.533 27.986,19.514 27.607,19.376 27.300 C 19.283 27.093,18.381 26.155,15.493 23.265 L 11.730 19.500 20.224 19.500 C 29.767 19.500,29.070 19.534,29.552 19.052 C 30.138 18.465,30.138 17.535,29.552 16.948 C 29.070 16.466,29.767 16.500,20.224 16.500 L 11.730 16.500 15.489 12.735 C 17.557 10.664,19.294 8.879,19.349 8.768 C 19.707 8.052,19.373 7.107,18.639 6.758 C 18.379 6.635,17.861 6.580,17.640 6.653 "
      fillRule=#evenodd
    />
  </Svg>
