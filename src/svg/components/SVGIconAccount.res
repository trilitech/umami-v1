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
      d="M1.009 11.150 L 1.020 18.300 1.113 18.549 C 1.321 19.102,1.767 19.620,2.235 19.850 L 2.498 19.980 11.919 19.991 C 21.294 20.001,21.341 20.001,21.609 19.920 C 22.143 19.758,22.676 19.196,22.888 18.571 L 22.980 18.300 22.991 11.150 L 23.001 4.000 12.000 4.000 L 0.999 4.000 1.009 11.150 M21.000 7.000 L 21.000 8.000 12.000 8.000 L 3.000 8.000 3.000 7.000 L 3.000 6.000 12.000 6.000 L 21.000 6.000 21.000 7.000 M20.991 14.130 C 20.981 17.059,20.975 17.269,20.908 17.392 C 20.807 17.577,20.567 17.808,20.379 17.901 C 20.223 17.979,20.082 17.980,12.060 17.991 C 5.900 18.000,3.858 17.991,3.728 17.956 C 3.487 17.890,3.265 17.716,3.133 17.492 L 3.020 17.300 3.009 14.150 L 2.998 11.000 12.000 11.000 L 21.002 11.000 20.991 14.130 "
      fillRule=#evenodd
    />
  </Svg>
