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
      <G transform="translate(2 2)" fillRule=#nonzero>
        <Circle fill="#FF4500" cx={10.->Style.dp} cy={10.->Style.dp} r={10.->Style.dp} />
        <Path
          d="M16.667 10c0-.807-.655-1.462-1.462-1.462a1.4 1.4 0 00-1.006.41c-.994-.714-2.374-1.182-3.895-1.24l.667-3.123 2.164.456a1.042 1.042 0 002.081-.047c0-.573-.467-1.04-1.04-1.04-.41 0-.76.233-.925.584l-2.42-.515a.291.291 0 00-.2.035.285.285 0 00-.116.164l-.737 3.486c-1.556.046-2.948.503-3.953 1.24a1.476 1.476 0 00-1.006-.41 1.463 1.463 0 00-.597 2.795c-.023.14-.035.293-.035.445 0 2.245 2.608 4.058 5.836 4.058s5.837-1.813 5.837-4.058c0-.152-.012-.293-.035-.433.48-.234.842-.749.842-1.345zm-10 1.04c0-.572.468-1.04 1.04-1.04.574 0 1.042.468 1.042 1.04 0 .574-.468 1.042-1.041 1.042s-1.041-.468-1.041-1.041zm5.813 2.75c-.714.713-2.07.76-2.468.76-.398 0-1.766-.059-2.468-.76a.275.275 0 010-.386.275.275 0 01.386 0c.444.444 1.403.608 2.093.608s1.638-.164 2.094-.608a.275.275 0 01.386 0 .301.301 0 01-.023.385zm-.188-1.708A1.042 1.042 0 0112.291 10c.573 0 1.041.468 1.041 1.04 0 .574-.468 1.042-1.04 1.042z"
          fill="#FFF"
        />
      </G>
    </G>
  </Svg>
