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
      d="M11.300 2.025 C 7.871 2.309,4.947 4.176,3.260 7.160 C 2.689 8.169,2.291 9.366,2.093 10.668 C 2.003 11.263,2.003 12.737,2.093 13.332 C 2.436 15.596,3.376 17.489,4.931 19.053 C 6.470 20.601,8.416 21.569,10.668 21.906 C 11.213 21.988,12.661 21.998,13.200 21.925 C 15.982 21.545,18.364 20.163,19.998 17.981 C 22.334 14.863,22.639 10.608,20.770 7.220 C 19.225 4.418,16.501 2.536,13.360 2.100 C 12.907 2.037,11.690 1.993,11.300 2.025 M12.860 4.042 C 14.321 4.209,15.722 4.775,16.900 5.674 C 17.239 5.934,17.925 6.606,18.211 6.960 C 19.943 9.104,20.460 11.991,19.575 14.575 C 19.333 15.280,18.798 16.286,18.380 16.819 C 18.321 16.895,18.289 16.868,17.601 16.179 L 16.883 15.460 17.026 15.240 C 17.381 14.694,17.707 13.886,17.866 13.160 C 17.977 12.649,17.969 11.329,17.851 10.799 C 17.299 8.324,15.382 6.495,12.920 6.095 C 12.430 6.016,11.530 6.016,11.040 6.095 C 9.757 6.304,8.632 6.885,7.716 7.814 C 6.906 8.637,6.368 9.634,6.110 10.791 C 6.037 11.118,6.023 11.310,6.022 12.000 C 6.020 12.717,6.032 12.872,6.113 13.235 C 6.654 15.650,8.468 17.421,10.900 17.906 C 11.403 18.006,12.557 18.006,13.059 17.906 C 13.817 17.754,14.590 17.448,15.180 17.065 L 15.460 16.883 16.168 17.589 C 16.707 18.127,16.866 18.308,16.834 18.347 C 16.811 18.375,16.600 18.526,16.366 18.683 C 13.197 20.803,8.946 20.328,6.259 17.554 C 5.633 16.908,5.228 16.337,4.826 15.533 C 3.472 12.827,3.802 9.552,5.674 7.100 C 5.934 6.761,6.606 6.075,6.960 5.789 C 8.110 4.860,9.505 4.261,10.980 4.062 C 11.470 3.996,12.375 3.986,12.860 4.042 M12.800 8.101 C 14.800 8.525,16.182 10.413,15.958 12.418 C 15.934 12.636,15.868 12.960,15.812 13.138 C 15.701 13.491,15.460 14.000,15.403 14.000 C 15.384 14.000,15.046 13.669,14.653 13.264 L 13.938 12.529 13.953 12.106 C 13.966 11.760,13.954 11.634,13.885 11.407 C 13.534 10.253,12.201 9.681,11.127 10.223 C 10.640 10.469,10.310 10.833,10.127 11.328 C 9.698 12.487,10.462 13.775,11.690 13.966 C 11.909 14.000,12.058 14.001,12.245 13.970 L 12.500 13.927 13.250 14.673 C 13.662 15.084,14.000 15.430,14.000 15.443 C 14.000 15.488,13.422 15.750,13.100 15.851 C 11.695 16.294,10.093 15.859,9.059 14.753 C 8.548 14.208,8.194 13.511,8.036 12.740 C 7.972 12.431,7.972 11.609,8.036 11.300 C 8.398 9.537,9.762 8.260,11.516 8.039 C 11.842 7.998,12.453 8.027,12.800 8.101 "
      fillRule=#evenodd
    />
  </Svg>
