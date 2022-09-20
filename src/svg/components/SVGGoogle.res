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
    <G fill="none" fillRule=#evenodd>
      <Path
        d="M22 12.227c0-.709-.065-1.39-.186-2.045h-9.61v3.868h5.492c-.237 1.25-.956 2.31-2.036 3.018v2.51h3.297c1.93-1.742 3.043-4.305 3.043-7.35z"
        fill="#4285F4"
      />
      <Path
        d="M12.204 22c2.755 0 5.065-.895 6.753-2.423l-3.297-2.509c-.914.6-2.083.955-3.456.955-2.658 0-4.907-1.76-5.71-4.123H3.085v2.59c1.68 3.27 5.13 5.51 9.12 5.51z"
        fill="#34A853"
      />
      <Path
        d="M6.494 13.9a5.9 5.9 0 01-.32-1.9c0-.66.116-1.3.32-1.9V7.51H3.085a9.834 9.834 0 000 8.98l3.41-2.59z"
        fill="#FBBC05"
      />
      <Path
        d="M12.204 5.977c1.498 0 2.843.505 3.9 1.496l2.928-2.868C17.264 2.99 14.955 2 12.204 2c-3.989 0-7.44 2.24-9.119 5.51l3.41 2.59c.802-2.364 3.051-4.123 5.71-4.123z"
        fill="#EA4335"
      />
      <Path d="M0 0h24v24H0z" />
    </G>
  </Svg>
