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
      d="M11.760 7.645 C 11.627 7.698,7.863 11.451,7.715 11.678 C 7.578 11.889,7.566 12.084,7.680 12.270 C 7.832 12.519,8.118 12.619,8.387 12.517 C 8.449 12.494,9.162 11.815,9.970 11.009 L 11.440 9.542 11.440 12.882 C 11.440 16.040,11.444 16.229,11.513 16.358 C 11.711 16.727,12.289 16.727,12.487 16.358 C 12.556 16.229,12.560 16.040,12.560 12.881 L 12.560 9.541 14.030 11.008 C 14.838 11.815,15.551 12.495,15.613 12.518 C 16.167 12.726,16.634 12.127,16.284 11.656 C 16.082 11.384,12.345 7.686,12.227 7.642 C 12.090 7.591,11.894 7.592,11.760 7.645 "
      fillRule=#evenodd
    />
  </Svg>
