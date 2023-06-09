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
      d="M11.260 2.023 C 11.194 2.032,10.969 2.059,10.760 2.083 C 9.020 2.281,7.283 3.007,5.800 4.154 C 5.361 4.494,4.492 5.364,4.151 5.805 C 3.042 7.237,2.373 8.804,2.092 10.628 C 2.000 11.228,2.000 12.772,2.092 13.372 C 2.540 16.281,4.063 18.708,6.440 20.300 C 7.761 21.184,9.116 21.693,10.780 21.929 C 11.320 22.005,12.790 21.993,13.380 21.906 C 14.871 21.688,16.303 21.142,17.520 20.327 C 19.987 18.675,21.504 16.230,21.928 13.220 C 21.995 12.746,21.994 11.248,21.926 10.760 C 21.665 8.885,20.940 7.174,19.807 5.760 C 18.211 3.768,15.926 2.463,13.400 2.099 C 12.968 2.036,11.531 1.986,11.260 2.023 M12.766 4.041 C 14.584 4.221,16.241 4.987,17.554 6.256 C 20.049 8.664,20.704 12.394,19.179 15.508 C 17.955 18.010,15.542 19.684,12.766 19.959 C 11.059 20.128,9.260 19.710,7.808 18.808 C 4.301 16.628,3.009 12.175,4.817 8.500 C 5.223 7.673,5.616 7.111,6.238 6.466 C 7.950 4.693,10.348 3.802,12.766 4.041 M8.660 8.652 C 8.554 8.758,8.522 8.828,8.502 9.002 C 8.487 9.122,8.481 10.553,8.488 12.182 C 8.501 15.449,8.486 15.257,8.749 15.429 C 8.849 15.495,9.064 15.500,12.000 15.500 C 14.936 15.500,15.151 15.495,15.251 15.429 C 15.514 15.257,15.499 15.449,15.512 12.182 C 15.519 10.553,15.513 9.122,15.498 9.002 C 15.478 8.828,15.446 8.758,15.340 8.652 L 15.208 8.520 12.000 8.520 L 8.792 8.520 8.660 8.652 M13.920 10.080 C 13.998 10.158,14.000 10.213,14.000 12.000 C 14.000 13.787,13.998 13.842,13.920 13.920 C 13.842 13.998,13.787 14.000,12.000 14.000 C 10.213 14.000,10.158 13.998,10.080 13.920 C 10.002 13.842,10.000 13.787,10.000 12.000 C 10.000 10.213,10.002 10.158,10.080 10.080 C 10.158 10.002,10.213 10.000,12.000 10.000 C 13.787 10.000,13.842 10.002,13.920 10.080 "
      fillRule=#evenodd
    />
  </Svg>
