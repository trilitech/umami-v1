/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

include Belt.Option;

let map2 = (opt1, opt2, f) =>
  switch (opt1, opt2) {
  | (Some(v1), Some(v2)) => Some(f(v1, v2))
  | _ => None
  };

let keep = (opt1, opt2) =>
  switch (opt1, opt2) {
  | (Some(v), _)
  | (_, Some(v)) => Some(v)
  | _ => None
  };

/* Specialized version where the result of map is always of the same type of the
   option's value */
let mapOrKeep = (opt1, opt2, f) => {
  let res = map2(opt1, opt2, f);
  res->isNone ? keep(opt1, opt2) : res;
};

let iter = (o, f) => {
  switch (o) {
  | Some(v) => f(v)
  | None => ()
  };
};

let onlyIf = (b, f) => b ? Some(f()) : None;

let rec firstSome = l => {
  switch (l) {
  | [] => None
  | [Some(_) as h, ..._] => h
  | [None, ...t] => firstSome(t)
  };
};

let both = (o1: option('a), o2: option('b)): option(('a, 'b)) =>
  switch (o1, o2) {
  | (Some(o1), Some(o2)) => Some((o1, o2))
  | (None, _)
  | (_, None) => None
  };

let default = Belt.Option.getWithDefault;
let mapDefault = Belt.Option.mapWithDefault;
