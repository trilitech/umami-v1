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
      d="M11.271 2.041 C 10.401 2.104,9.385 2.322,8.577 2.620 C 7.977 2.840,7.029 3.320,6.517 3.662 C 5.328 4.456,4.285 5.521,3.534 6.709 C 3.242 7.170,2.766 8.155,2.585 8.672 C 1.919 10.570,1.840 12.626,2.357 14.583 C 2.671 15.775,3.270 16.993,4.054 18.040 C 4.407 18.511,5.220 19.351,5.700 19.741 C 6.766 20.607,8.127 21.303,9.417 21.643 C 11.794 22.271,14.270 22.020,16.448 20.931 C 18.078 20.115,19.488 18.839,20.466 17.291 C 20.761 16.824,21.238 15.839,21.416 15.329 C 22.166 13.179,22.165 10.811,21.415 8.671 C 21.234 8.155,20.758 7.170,20.466 6.709 C 19.923 5.850,19.067 4.882,18.300 4.259 C 17.471 3.586,16.288 2.920,15.320 2.581 C 14.071 2.144,12.586 1.946,11.271 2.041 M13.560 4.139 C 16.961 4.852,19.492 7.554,19.940 10.952 C 20.061 11.864,19.999 13.013,19.783 13.900 C 19.403 15.456,18.494 16.964,17.309 18.003 C 15.827 19.302,13.972 20.000,12.000 20.000 C 9.913 20.000,8.005 19.239,6.454 17.789 C 5.631 17.019,4.831 15.776,4.441 14.660 C 3.853 12.980,3.848 11.065,4.428 9.380 C 5.403 6.549,7.912 4.491,10.920 4.058 C 11.184 4.020,11.570 4.008,12.180 4.019 C 12.936 4.032,13.130 4.049,13.560 4.139 M11.500 7.044 C 9.474 7.227,7.698 8.749,7.179 10.748 C 6.407 13.720,8.505 16.690,11.566 16.959 C 13.972 17.170,16.213 15.594,16.818 13.264 C 17.594 10.275,15.503 7.313,12.424 7.040 C 12.180 7.018,11.953 7.003,11.920 7.006 C 11.887 7.009,11.698 7.026,11.500 7.044 "
      fillRule=#evenodd
    />
  </Svg>
