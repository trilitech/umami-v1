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
      d="M11.220 4.024 C 10.146 4.132,9.836 4.182,9.115 4.363 C 5.578 5.246,2.585 7.798,1.140 11.163 L 0.993 11.506 1.223 12.017 C 2.705 15.307,5.642 17.770,9.120 18.639 C 10.172 18.902,10.848 18.980,12.040 18.978 C 13.305 18.976,14.097 18.867,15.280 18.532 C 18.621 17.587,21.472 15.068,22.860 11.837 L 23.007 11.494 22.777 10.983 C 21.092 7.243,17.607 4.646,13.540 4.100 C 13.085 4.039,11.563 3.989,11.220 4.024 M12.920 6.042 C 14.085 6.148,15.246 6.480,16.340 7.018 C 17.615 7.646,18.633 8.445,19.553 9.540 C 19.909 9.965,20.484 10.828,20.676 11.229 L 20.811 11.512 20.604 11.900 C 20.161 12.729,19.553 13.525,18.815 14.241 C 17.505 15.511,15.893 16.380,14.140 16.761 C 13.325 16.938,12.930 16.978,12.000 16.978 C 10.885 16.978,10.218 16.886,9.202 16.593 C 8.083 16.271,6.832 15.625,5.907 14.891 C 4.926 14.112,3.861 12.834,3.352 11.823 L 3.192 11.507 3.286 11.304 C 3.437 10.977,3.858 10.302,4.170 9.886 C 4.518 9.423,5.245 8.664,5.740 8.247 C 6.898 7.274,8.482 6.518,10.027 6.203 C 10.978 6.009,11.957 5.954,12.920 6.042 M11.280 7.062 C 9.559 7.334,8.129 8.627,7.651 10.345 C 7.548 10.712,7.541 10.793,7.541 11.500 C 7.541 12.205,7.548 12.288,7.650 12.651 C 8.273 14.885,10.414 16.286,12.680 15.942 C 13.997 15.742,15.180 14.940,15.870 13.779 C 16.136 13.330,16.293 12.923,16.402 12.397 C 16.523 11.809,16.497 10.878,16.344 10.328 C 15.725 8.105,13.552 6.702,11.280 7.062 M12.535 9.060 C 13.404 9.244,14.194 10.002,14.417 10.866 C 14.497 11.176,14.499 11.826,14.421 12.120 C 14.308 12.544,14.089 12.918,13.764 13.243 C 13.233 13.772,12.745 13.978,12.020 13.979 C 11.765 13.980,11.496 13.957,11.380 13.924 C 10.525 13.687,9.797 12.961,9.583 12.134 C 9.501 11.815,9.501 11.185,9.583 10.866 C 9.804 10.012,10.594 9.246,11.447 9.061 C 11.782 8.989,12.197 8.988,12.535 9.060 "
      fillRule=#evenodd
    />
  </Svg>
