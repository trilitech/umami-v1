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
      d="M4.768 2.043 C 4.398 2.091,4.134 2.177,3.714 2.387 C 3.400 2.544,3.278 2.635,2.955 2.958 C 2.614 3.298,2.547 3.390,2.366 3.761 C 2.062 4.387,2.035 4.534,2.011 5.730 L 1.991 6.760 4.375 6.760 L 6.760 6.760 6.760 4.380 L 6.760 2.000 5.890 2.005 C 5.412 2.008,4.906 2.025,4.768 2.043 M9.640 8.159 L 9.640 14.320 15.801 14.320 L 21.963 14.320 21.950 9.510 C 21.936 4.724,21.936 4.698,21.850 4.400 C 21.803 4.235,21.678 3.929,21.574 3.720 C 21.413 3.399,21.324 3.281,21.002 2.960 C 20.569 2.529,20.144 2.276,19.580 2.114 L 19.260 2.022 14.450 2.009 L 9.640 1.997 9.640 8.159 M2.000 12.000 L 2.000 14.360 4.380 14.360 L 6.760 14.360 6.760 12.000 L 6.760 9.640 4.380 9.640 L 2.000 9.640 2.000 12.000 M2.011 18.270 C 2.034 19.460,2.058 19.594,2.369 20.239 C 2.548 20.612,2.612 20.700,2.956 21.044 C 3.300 21.388,3.388 21.452,3.761 21.631 C 4.406 21.942,4.540 21.966,5.730 21.989 L 6.760 22.009 6.760 19.625 L 6.760 17.240 4.375 17.240 L 1.991 17.240 2.011 18.270 M9.640 19.620 L 9.640 22.000 12.000 22.000 L 14.360 22.000 14.360 19.620 L 14.360 17.240 12.000 17.240 L 9.640 17.240 9.640 19.620 M17.240 19.605 L 17.240 21.969 18.270 21.949 C 19.131 21.931,19.347 21.915,19.589 21.848 C 20.552 21.583,21.292 20.958,21.699 20.067 C 21.937 19.546,21.967 19.361,21.989 18.250 L 22.010 17.240 19.625 17.240 L 17.240 17.240 17.240 19.605 "
      fillRule=#evenodd
    />
  </Svg>
