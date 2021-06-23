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

let getOk = (result, sink) =>
  switch (result) {
  | Ok(value) => sink(value)
  | Error(_) => ()
  };

let getError = (result, sink) =>
  switch (result) {
  | Ok(_) => ()
  | Error(value) => sink(value)
  };

let fromOption = (v, error) =>
  switch (v) {
  | Some(v) => Ok(v)
  | None => error
  };

let flatMap2 = (r1, r2, f) =>
  switch (r1, r2) {
  | (Ok(v1), Ok(v2)) => f(v1, v2)
  | (Error(err), _)
  | (_, Error(err)) => Error(err)
  };

let mapError = (r, f) =>
  switch (r) {
  | Ok(v) => Ok(v)
  | Error(e) => f(e)
  };

let map2 = (r1, r2, f) => flatMap2(r1, r2, (v1, v2) => Ok(f(v1, v2)));

let collect = (type err, l: list(result(_, err))) => {
  // let's quit the reduce as soon as e have an error
  exception Fail(err);
  try(
    l
    // This uses reduce + reverse to always catch the first error in the list
    ->List.reduce([], (l, v) =>
        switch (v) {
        | Ok(v) => [v, ...l]
        | Error(e) => raise(Fail(e))
        }
      )
    ->List.reverse
    ->Ok
  ) {
  | Fail(err) => Error(err)
  };
};

let collectArray = (type err, arr: array(result(_, err))) => {
  // let's quit the reduce as soon as e have an error
  exception Fail(err);
  try(
    arr
    // This uses reduce + reverse to always catch the first error in the list
    ->Array.map(v =>
        switch (v) {
        | Ok(v) => v
        | Error(e) => raise(Fail(e))
        }
      )
    ->Ok
  ) {
  | Fail(err) => Error(err)
  };
};
