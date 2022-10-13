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
      <Path d="M0 0h24v24H0z" />
      <Path
        d="M18.93 5.743a16.69 16.69 0 00-4.07-1.242.062.062 0 00-.066.03c-.175.308-.37.71-.506 1.025a15.471 15.471 0 00-4.573 0A10.3 10.3 0 009.2 4.531a.065.065 0 00-.065-.03 16.643 16.643 0 00-4.07 1.242.058.058 0 00-.028.023c-2.593 3.812-3.303 7.53-2.954 11.201a.067.067 0 00.026.046 16.692 16.692 0 004.994 2.484.065.065 0 00.07-.022c.385-.517.728-1.062 1.022-1.636a.062.062 0 00-.035-.086 11.001 11.001 0 01-1.56-.732.063.063 0 01-.007-.105c.105-.077.21-.157.31-.239a.063.063 0 01.065-.008c3.273 1.47 6.817 1.47 10.051 0a.062.062 0 01.066.008c.1.08.205.162.31.24a.063.063 0 01-.005.104c-.499.287-1.017.529-1.561.731a.062.062 0 00-.034.087c.3.573.643 1.118 1.02 1.635a.064.064 0 00.07.023 16.636 16.636 0 005.003-2.484.063.063 0 00.026-.045c.417-4.245-.699-7.932-2.957-11.201a.05.05 0 00-.026-.024zM8.684 14.731c-.985 0-1.797-.89-1.797-1.983s.796-1.983 1.797-1.983c1.01 0 1.814.898 1.798 1.983 0 1.093-.796 1.983-1.798 1.983zm6.646 0c-.985 0-1.797-.89-1.797-1.983s.796-1.983 1.797-1.983c1.009 0 1.813.898 1.797 1.983 0 1.093-.788 1.983-1.797 1.983z"
        fill="#5865F2"
        fillRule=#nonzero
      />
    </G>
  </Svg>
