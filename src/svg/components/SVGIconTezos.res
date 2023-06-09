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
  <Svg viewBox="0 0 19 19" ?width ?height ?fill ?stroke>
    <Path
      d="M6.618 3.360 L 6.618 3.553 6.824 3.775 L 7.030 3.998 7.030 4.849 L 7.030 5.700 6.096 5.700 L 5.162 5.700 5.162 6.064 L 5.162 6.428 6.096 6.428 L 7.030 6.428 7.031 8.574 C 7.031 10.954,7.032 10.970,7.235 11.381 C 7.400 11.715,7.591 11.917,7.864 12.046 C 8.426 12.313,9.292 12.364,9.769 12.159 C 10.233 11.960,10.703 11.355,10.703 10.958 C 10.703 10.818,10.689 10.779,10.611 10.701 C 10.468 10.558,10.409 10.578,10.227 10.829 C 10.037 11.090,9.764 11.341,9.564 11.440 C 9.450 11.497,9.366 11.511,9.149 11.511 C 8.906 11.511,8.864 11.502,8.746 11.424 C 8.669 11.372,8.576 11.268,8.520 11.171 C 8.332 10.842,8.328 10.787,8.328 8.504 L 8.328 6.428 10.183 6.428 C 11.658 6.428,12.032 6.436,12.013 6.468 C 11.999 6.490,11.399 7.352,10.679 8.383 L 9.370 10.259 9.380 10.415 L 9.389 10.571 9.848 10.322 C 10.481 9.979,10.752 9.917,11.158 10.023 C 11.633 10.146,12.029 10.469,12.237 10.903 C 12.612 11.682,12.659 12.885,12.353 13.870 C 12.179 14.431,11.750 14.864,11.217 15.017 C 11.043 15.067,10.941 15.075,10.656 15.061 C 10.141 15.037,9.742 14.903,9.541 14.688 L 9.471 14.612 9.620 14.502 C 9.940 14.267,10.056 13.766,9.882 13.371 C 9.748 13.067,9.503 12.914,9.149 12.914 C 8.797 12.914,8.542 13.076,8.402 13.388 C 8.332 13.545,8.332 13.979,8.404 14.250 C 8.550 14.807,8.949 15.319,9.416 15.549 C 10.406 16.035,11.873 15.875,12.755 15.185 C 13.679 14.462,14.077 12.895,13.709 11.426 C 13.571 10.876,13.239 10.284,12.891 9.969 C 12.644 9.746,12.242 9.510,11.908 9.393 C 11.680 9.313,11.590 9.298,11.337 9.299 C 11.131 9.299,10.992 9.316,10.896 9.352 C 10.820 9.382,10.754 9.402,10.750 9.397 C 10.746 9.393,11.412 8.672,12.230 7.797 L 13.717 6.204 13.470 5.932 L 13.224 5.660 10.776 5.684 L 8.328 5.707 8.328 4.565 L 8.328 3.422 8.099 3.295 L 7.869 3.168 7.244 3.167 L 6.618 3.167 6.618 3.360 "
      fillRule=#evenodd
    />
  </Svg>
