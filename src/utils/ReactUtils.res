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

let mapOpt = (v, f) => v->Option.mapWithDefault(React.null, f)

let hideNil = (v, f) =>
  switch v {
  | list{} => React.null
  | v => f(v)
  }

let opt = e =>
  switch e {
  | Some(e) => e
  | None => React.null
  }

let displayOn = b => {
  open Style
  style(~display=b ? #flex : #none, ())
}

let visibleOn = b => {
  open Style
  style(~opacity=b ? 1. : 0., ())
}

let onlyWhen = (elt, b) => b ? elt : React.null

let startFade = (refval, endval, duration, endCallback) => {
  open Animated
  open Value.Timing
  timing(
    refval,
    config(~toValue=fromRawValue(endval), ~duration, ~useNativeDriver=true, ()),
  )->start(~endCallback?, ())
}

let useIsMounted = () => {
  let (isMounted, setIsMounted) = React.useState(_ => false)
  React.useEffect1(() => {
    setIsMounted(_ => true)
    None
  }, [setIsMounted])
  isMounted
}

let styles = l => Style.array(List.toArray(l))

module Next = {
  type t<'value> =
    | Empty
    | Pending('value)
    | Processing

  type action<'value> =
    | Next
    | Done
    | Send('value)

  let reducer = (state, action) =>
    switch (state, action) {
    | (Pending(_), Next) => Processing
    | (Processing, Done) => Empty
    | (Empty | Pending(_), Send(value)) => Pending(value)
    | _ => state
    }

  let value = state =>
    switch state {
    | Pending(value) => Some(value)
    | _ => None
    }
}

let useNextState = () => {
  let (state, dispatch) = React.useReducer(Next.reducer, Empty)
  let sender = React.useRef(_ => ())

  let next = () => {
    dispatch(Next)
    state->Next.value
  }

  sender.current = value => dispatch(Send(value))

  (next, () => dispatch(Done), value => sender.current(value))
}

let mkAsyncEffect = (reactf, f, dp) => reactf(() => {
    f->Promise.async
    None
  }, dp)

let useAsyncEffect1 = (f: unit => Promise.t<_>, dp) => mkAsyncEffect(React.useEffect1, f, dp)

let useAsyncEffect2 = (f: unit => Promise.t<_>, dp) => mkAsyncEffect(React.useEffect2, f, dp)
