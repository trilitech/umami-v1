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
      d="M11.123 2.041 C 8.389 2.306,5.946 3.604,4.242 5.696 C 3.611 6.472,2.964 7.603,2.625 8.523 C 2.420 9.080,2.212 9.898,2.113 10.540 C 2.000 11.277,1.990 12.620,2.093 13.340 C 2.533 16.412,4.294 19.058,6.940 20.620 C 9.200 21.954,11.981 22.335,14.539 21.663 C 16.526 21.140,18.329 20.005,19.627 18.460 C 22.777 14.710,22.777 9.290,19.627 5.540 C 18.772 4.523,17.628 3.631,16.426 3.043 C 15.452 2.568,14.509 2.272,13.472 2.118 C 12.855 2.026,11.681 1.988,11.123 2.041 M12.860 4.042 C 14.321 4.209,15.722 4.775,16.900 5.674 C 17.239 5.934,17.925 6.606,18.211 6.960 C 18.773 7.655,19.273 8.553,19.554 9.367 C 20.441 11.946,19.988 14.723,18.326 16.900 C 18.066 17.239,17.394 17.925,17.040 18.211 C 14.102 20.584,9.960 20.596,7.000 18.239 C 6.616 17.933,5.949 17.260,5.674 16.900 C 3.443 13.978,3.443 10.022,5.674 7.100 C 5.934 6.761,6.606 6.075,6.960 5.789 C 8.110 4.860,9.505 4.261,10.980 4.062 C 11.470 3.996,12.375 3.986,12.860 4.042 M7.710 7.710 L 7.000 8.420 8.790 10.210 L 10.580 12.000 8.790 13.790 L 7.000 15.580 7.710 16.290 L 8.420 17.000 10.210 15.210 L 12.000 13.420 13.790 15.210 L 15.580 17.000 16.290 16.290 L 17.000 15.580 15.210 13.790 L 13.420 12.000 15.210 10.210 L 17.000 8.420 16.290 7.710 L 15.580 7.000 13.790 8.790 L 12.000 10.580 10.210 8.790 L 8.420 7.000 7.710 7.710 "
      fillRule=#evenodd
    />
  </Svg>
