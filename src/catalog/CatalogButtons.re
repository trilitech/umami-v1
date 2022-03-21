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

open CatalogUtils;

[@react.component]
let make = () => {
  <>
    {displayElement(
       ~title="<Buttons.FormPrimary/>",
       ~subtitle="nominal",
       ~element=<Buttons.FormPrimary text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.FormPrimary/>",
       ~subtitle="disabled",
       ~element=
         <Buttons.FormPrimary text="hello" disabled=true onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.FormPrimary/>",
       ~subtitle="loading",
       ~element=
         <Buttons.FormPrimary text="hello" loading=true onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.FormSecondary/>",
       ~subtitle="nominal",
       ~element=<Buttons.FormSecondary text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.SubmitPrimary/>",
       ~subtitle="nominal",
       ~element=<Buttons.SubmitPrimary text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.SubmitPrimary/>",
       ~subtitle="disabled",
       ~element=
         <Buttons.SubmitPrimary
           text="hello"
           disabled=true
           onPress={_ => ()}
         />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.SubmitSecondary/>",
       ~subtitle="nominal",
       ~element=<Buttons.SubmitSecondary text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.SubmitTertiary/>",
       ~subtitle="nominal",
       ~element=<Buttons.SubmitTertiary text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.SubmitTertiaryHighEmphasis/>",
       ~subtitle="nominal",
       ~element=
         <Buttons.SubmitTertiaryHighEmphasis text="hello" onPress={_ => ()} />,
       (),
     )}
    {displayElement(
       ~title="<Buttons.RightArrowButton/>",
       ~subtitle="nominal",
       ~element=<Buttons.RightArrowButton text="hello" onPress={_ => ()} />,
       (),
     )}
  </>;
};
