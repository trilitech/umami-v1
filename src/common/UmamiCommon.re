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

module Infix = {
  let (>>=) = Future.(>>=);
};

include Infix;

module Lib = {
  module Future = {
    let fromPromise = p =>
      Future.make(resolve => {
        p
        |> Js.Promise.then_(v => {
             resolve(Ok(v));
             Js.Promise.resolve();
           })
      });
  };

  module String = {
    let countLeading = (str, c) => {
      let rec loop = n => str->Js.String2.charAt(n) == c ? loop(n + 1) : n;
      loop(0);
    };
  };

  module Option = {
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
  };

  module Result = {
    let iterOk = (r, f) => {
      switch (r) {
      | Ok(v) => f(v)
      | Error(_) => ()
      };
    };

    let fromOption = (v: option('a), e) =>
      switch (v) {
      | None => Error(e)
      | Some(v) => Ok(v)
      };
  };

  module List = {
    let add = (l, e) => [e, ...l];

    let reduceGroupBy =
        (l: list('a), ~group: 'a => 'g, ~map: (option('b), 'a) => 'b)
        : list(('g, 'b)) =>
      l->List.reduce(
        [],
        (acc, e) => {
          let group: 'g = e->group;
          let groupValue = acc->List.getAssoc(group, (==));
          acc->List.setAssoc(group, map(groupValue, e), (==));
        },
      );

    let addOpt = (l, e) =>
      switch (e) {
      | None => l
      | Some(e) => [e, ...l]
      };

    let rec firsts = (l, n) => {
      switch (l) {
      | [] => l
      | [h, ...t] => n == 0 ? [] : t->firsts(n - 1)->List.add(h)
      };
    };

    let rec findMap = (l, f) => {
      switch (l) {
      | [] => None
      | [h, ...t] =>
        switch (f(h)) {
        | Some(v) => Some(v)
        | None => findMap(t, f)
        }
      };
    };
  };

  let tap = (x, f) => {
    f(x);
    x;
  };
};
