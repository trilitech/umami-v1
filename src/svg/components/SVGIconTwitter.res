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
      d="M15.340 4.046 C 14.226 4.187,13.175 4.803,12.520 5.698 C 12.263 6.049,12.018 6.567,11.894 7.020 C 11.802 7.359,11.785 7.502,11.783 7.960 C 11.781 8.257,11.795 8.606,11.814 8.736 L 11.847 8.971 11.454 8.943 C 10.453 8.871,9.125 8.562,8.140 8.172 C 6.483 7.516,5.174 6.635,3.923 5.334 C 3.313 4.699,3.375 4.704,3.146 5.276 C 2.934 5.806,2.867 6.174,2.870 6.800 C 2.871 7.248,2.890 7.408,2.976 7.740 C 3.205 8.619,3.742 9.444,4.395 9.919 C 4.530 10.018,4.640 10.108,4.640 10.121 C 4.640 10.151,4.109 10.098,3.880 10.045 C 3.620 9.985,3.165 9.825,2.969 9.725 C 2.776 9.626,2.785 9.599,2.841 10.100 C 3.027 11.733,4.170 13.080,5.755 13.533 C 5.940 13.585,6.075 13.638,6.055 13.650 C 5.880 13.755,4.680 13.826,4.390 13.748 C 4.234 13.706,4.245 13.763,4.482 14.236 C 5.114 15.498,6.369 16.360,7.731 16.468 C 7.930 16.483,8.038 16.508,8.028 16.536 C 8.000 16.623,7.209 17.124,6.700 17.377 C 5.832 17.807,4.866 18.096,3.938 18.201 C 3.716 18.226,3.209 18.241,2.811 18.233 C 2.413 18.226,2.108 18.234,2.134 18.251 C 2.602 18.564,3.636 19.056,4.374 19.315 C 5.691 19.777,6.863 19.970,8.360 19.969 C 11.041 19.969,13.461 19.176,15.433 17.649 C 18.178 15.525,19.824 12.265,19.947 8.708 L 19.973 7.957 20.296 7.702 C 20.807 7.301,21.499 6.575,21.876 6.046 L 21.966 5.919 21.753 6.005 C 21.193 6.232,19.958 6.545,19.755 6.512 C 19.701 6.503,19.771 6.433,20.003 6.262 C 20.367 5.994,20.683 5.669,20.923 5.316 C 21.101 5.054,21.423 4.409,21.391 4.378 C 21.381 4.367,21.131 4.474,20.836 4.616 C 20.272 4.887,19.659 5.106,19.145 5.220 L 18.837 5.289 18.628 5.092 C 18.021 4.519,17.182 4.143,16.280 4.040 C 15.890 3.995,15.731 3.996,15.340 4.046 "
      fillRule=#evenodd
    />
  </Svg>
