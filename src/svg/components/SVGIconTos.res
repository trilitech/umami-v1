/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com> */
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
  <Svg viewBox="0 0 19.2 19.2" ?width ?height ?fill ?stroke>
    <Path
      d="M6.000 2.398 L 2.416 3.993 2.407 6.516 C 2.395 9.666,2.420 10.066,2.705 11.200 C 3.385 13.911,5.191 16.309,7.504 17.573 C 8.202 17.955,9.303 18.377,9.600 18.378 C 9.776 18.379,10.493 18.142,11.008 17.914 C 13.411 16.849,15.363 14.652,16.262 12.000 C 16.406 11.573,16.546 11.038,16.636 10.560 C 16.778 9.814,16.784 9.651,16.784 6.756 L 16.784 3.993 13.200 2.398 C 11.229 1.521,9.609 0.803,9.600 0.803 C 9.591 0.803,7.971 1.521,6.000 2.398 M12.409 3.800 L 15.204 5.039 15.191 7.312 C 15.177 9.729,15.175 9.765,15.007 10.542 C 14.848 11.273,14.570 12.061,14.253 12.674 L 14.102 12.966 13.527 12.391 L 12.952 11.816 13.164 11.388 C 13.491 10.726,13.631 10.072,13.589 9.391 C 13.476 7.545,12.169 6.046,10.352 5.679 C 10.000 5.607,9.275 5.598,8.928 5.660 C 8.097 5.809,7.368 6.192,6.766 6.797 C 6.186 7.380,5.821 8.067,5.660 8.880 C 5.595 9.212,5.603 10.059,5.675 10.393 C 5.852 11.209,6.191 11.832,6.780 12.420 C 7.366 13.006,7.993 13.348,8.807 13.525 C 9.002 13.568,9.210 13.583,9.600 13.583 C 10.160 13.583,10.442 13.541,10.874 13.394 C 11.107 13.315,11.496 13.136,11.689 13.019 L 11.810 12.946 12.502 13.638 L 13.194 14.330 12.701 14.823 C 12.152 15.372,11.641 15.773,11.072 16.103 C 10.696 16.322,10.115 16.586,9.781 16.691 L 9.595 16.749 9.322 16.657 C 8.920 16.520,8.163 16.137,7.754 15.864 C 7.273 15.541,6.967 15.289,6.525 14.849 C 5.107 13.440,4.213 11.523,4.033 9.504 C 4.015 9.299,4.000 8.227,4.000 7.088 L 4.000 5.040 6.776 3.803 C 8.303 3.123,9.566 2.565,9.583 2.563 C 9.600 2.561,10.872 3.118,12.409 3.800 M10.224 7.281 C 11.035 7.501,11.698 8.169,11.925 8.992 C 12.004 9.279,12.004 9.921,11.925 10.208 C 11.696 11.038,11.038 11.696,10.208 11.925 C 10.060 11.965,9.885 11.982,9.600 11.982 C 8.996 11.982,8.553 11.831,8.109 11.476 C 7.411 10.916,7.073 10.006,7.246 9.151 C 7.441 8.187,8.147 7.464,9.104 7.251 C 9.380 7.189,9.945 7.204,10.224 7.281 "
      fillRule=#evenodd
    />
  </Svg>
