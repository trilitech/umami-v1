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
      d="M11.260 2.023 C 11.194 2.032,10.969 2.059,10.760 2.083 C 8.587 2.330,6.387 3.424,4.839 5.026 C 4.064 5.828,3.528 6.590,3.039 7.580 C 2.557 8.557,2.277 9.456,2.093 10.620 C 1.997 11.221,1.997 12.755,2.092 13.372 C 2.440 15.632,3.373 17.497,4.938 19.062 C 5.776 19.900,6.562 20.463,7.620 20.982 C 8.567 21.446,9.509 21.736,10.628 21.908 C 11.250 22.003,12.734 22.002,13.380 21.906 C 14.794 21.696,16.023 21.256,17.220 20.531 C 18.454 19.784,19.671 18.577,20.496 17.280 C 21.003 16.485,21.524 15.218,21.738 14.265 C 21.934 13.391,21.974 13.005,21.974 12.000 C 21.974 10.995,21.934 10.609,21.738 9.735 C 21.524 8.782,21.003 7.515,20.496 6.720 C 19.671 5.423,18.454 4.216,17.220 3.469 C 16.027 2.746,14.776 2.298,13.400 2.099 C 12.968 2.036,11.532 1.986,11.260 2.023 M13.360 4.122 C 15.596 4.507,17.574 5.845,18.801 7.800 C 19.705 9.242,20.130 11.062,19.958 12.766 C 19.807 14.267,19.267 15.639,18.346 16.860 C 18.036 17.271,17.271 18.036,16.860 18.346 C 15.216 19.586,13.217 20.159,11.234 19.958 C 9.364 19.769,7.705 18.984,6.377 17.661 C 5.653 16.939,5.193 16.295,4.773 15.412 C 3.750 13.266,3.750 10.734,4.773 8.588 C 5.193 7.705,5.653 7.061,6.377 6.339 C 7.677 5.043,9.350 4.240,11.160 4.042 C 11.698 3.983,12.786 4.023,13.360 4.122 M8.710 8.710 L 8.000 9.420 9.290 10.710 L 10.580 12.000 9.290 13.290 L 8.000 14.580 8.710 15.290 L 9.420 16.000 10.710 14.710 L 12.000 13.420 13.290 14.710 L 14.580 16.000 15.290 15.290 L 16.000 14.580 14.710 13.290 L 13.420 12.000 14.710 10.710 L 16.000 9.420 15.290 8.710 L 14.580 8.000 13.290 9.290 L 12.000 10.580 10.710 9.290 L 9.420 8.000 8.710 8.710 "
      fillRule=#evenodd
    />
  </Svg>
