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

let getOk = (future, sink) =>
  future->Future.get(result => result->ResultEx.getOk(sink));

let getError = (future, sink) =>
  future->Future.get(result => result->ResultEx.getError(sink));

let fromOption = (option, ~error) =>
  Future.value(
    switch (option) {
    | Some(value) => Ok(value)
    | None => Error(error)
    },
  );

let fromOptionWithDefault = (option, ~default) =>
  Future.value(
    switch (option) {
    | Some(value) => Ok(value)
    | None => Ok(default)
    },
  );

let flatMap2 = (fa, fb, f) =>
  Future.flatMap(fa, a => Future.flatMap(fb, b => f(a, b)));

let flatMapOk2 = (fa, fb, f) =>
  flatMap2(fa, fb, (r1, r2) =>
    switch (r1, r2) {
    | (Ok(v1), Ok(v2)) => f(v1, v2)
    | (Error(e), _)
    | (_, Error(e)) => Future.value(Error(e))
    }
  );

let all = array =>
  array
  ->List.fromArray
  ->Future.all
  ->Future.map(results => Ok(results->List.toArray));

let fromCallback = (f, mapError) =>
  Future.make(resolve =>
    {
      (e, v) =>
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e->mapError)->resolve
        | None => Ok(v)->resolve
        };
    }
    ->f
  );

let fromUnitCallback = (f, mapError) =>
  Future.make(resolve =>
    {
      e =>
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e->mapError)->resolve
        | None => Ok()->resolve
        };
    }
    ->f
  );

let timeout = sec =>
  Future.make(resolve => {
    Js.Global.setTimeout(() => Ok()->resolve, sec)->ignore
  });

let ok = v => v->Ok->Future.value;
let err = v => v->Error->Future.value;
let some = v => v->Some->Ok->Future.value;
let none = () => None->Ok->Future.value;

let ignore = (ft: Future.t(Result.t(_, _))) => {
  ft->ignore;
};

let async = (f: unit => Future.t(Result.t(_, _))) => f()->ignore;
