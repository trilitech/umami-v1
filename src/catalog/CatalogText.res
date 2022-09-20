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

open CatalogUtils

// src/Greeting.res
@react.component
let make = () => <>
  {displayElement(
    ~element=<Typography.BigText> {"This is big text"->React.string} </Typography.BigText>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Headline> {"This is a headline"->React.string} </Typography.Headline>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Body1> {"This is body1 text"->React.string} </Typography.Body1>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Body2> {"This is body2 text"->React.string} </Typography.Body2>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Overline1>
      {"This is overline1 text"->React.string}
    </Typography.Overline1>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Overline2>
      {"This is overline2 text"->React.string}
    </Typography.Overline2>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Overline3>
      {"This is overline3 text"->React.string}
    </Typography.Overline3>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Address> {"This is address text"->React.string} </Typography.Address>,
    (),
  )}
  {displayElement(
    ~element=<Typography.Contract> {"This is contract text"->React.string} </Typography.Contract>,
    (),
  )}
  {displayElement(
    ~element=<Typography.ButtonPrimary>
      {"This is button primary text"->React.string}
    </Typography.ButtonPrimary>,
    (),
  )}
  {displayElement(
    ~element=<Typography.ButtonSecondary>
      {"This is button secondary text"->React.string}
    </Typography.ButtonSecondary>,
    (),
  )}
  {displayElement(
    ~element=<Typography.ButtonTernary>
      {"This is button ternary text"->React.string}
    </Typography.ButtonTernary>,
    (),
  )}
</>
