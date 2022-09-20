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
open Style

let formAction = style(
  ~flexDirection=#row,
  ~justifyContent=#spaceAround,
  ~width=300.->dp,
  ~marginTop=12.->dp,
  ~alignSelf=#center,
  (),
)

let formActionSpaceBetween = style(
  ~flexDirection=#row,
  ~justifyContent=#spaceBetween,
  ~marginTop=32.->dp,
  (),
)

let formSubmit = style(~marginTop=12.->dp, ())
let formSecondary = style(~marginTop=10.->dp, ())

let verticalFormAction = ReactUtils.styles(list{
  formAction,
  style(~flexDirection=#column, ~width=100.->pct, ()),
})

let header = style(~marginBottom=24.->dp, ~textAlign=#center, ())
let headerMarginBottom8 = style(~marginBottom=8.->dp, ~textAlign=#center, ())
let textContent = style(~marginBottom=24.->dp, ~textAlign=#center, ())

let headerWithoutMarginBottom = style(~marginBottom=4.->dp, ~textAlign=#center, ())
let subtitle = style(~marginTop=10.->dp, ())
let textAlignCenter = style(~textAlign=#center, ())

let amountRow = style(~display=#flex, ~flexDirection=#row, ~justifyContent=#spaceBetween, ())

let square = v => style(~width=v->dp, ~height=v->dp, ())

let onboarding = {
  open Style
  StyleSheet.create({
    "title": style(~marginBottom=8.->dp, ~textAlign=#center, ()),
    "stepPager": style(~marginBottom=4.->dp, ~textAlign=#center, ()),
    "stepTitle": style(~marginBottom=10.->dp, ~textAlign=#center, ()),
    "stepBody": style(~marginBottom=28.->dp, ~textAlign=#center, ()),
  })
}

let flexAlignedRow = style(~display=#flex, ~flexDirection=#row, ~alignItems=#center)

let section = {
  open Style
  StyleSheet.create({
    "spacing": style(~marginTop=24.->dp, ()),
    "submitSpacing": style(~marginTop=32.->dp, ()),
  })
}

let selector = {
  open Style
  StyleSheet.create({
    "item": style(
      ~display=#flex,
      ~alignItems=#center,
      ~marginLeft=16.->dp,
      ~flexDirection=#row,
      ~flexWrap=#wrap,
      (),
    ),
    "label": style(~marginBottom=4.->dp, ()),
    "selector": style(~marginBottom=24.->dp, ()),
    "button": style(~flex=1., ~paddingVertical=11.->dp, ()),
  })
}

let accountIcon = style(~justifyContent=#center, ~paddingRight=16.->dp, ())
