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

/* The goal of this module is to define custom let-binding to ease
   promise-oriented code integration. */

module Res = {
  /* [let%Res x = m; n] means:
     - [m] has type `Result.t('a, 'err)`
     - [x] is an identifier of type `'a`
     - [n] has type `Result.t('b, 'err)`
     - the whole expression has then type `Result.t('b, 'err)`

     It is strictly equivalent to `m->Result.flatMap(x => n)`
     */

  let let_: (result(_), _) => result(_) = Result.flatMap;
};

module ResMap = {
  /* [let%ResMap x = m; n] means:
     - [m] has type `Result.t('a, 'err)`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Result.t('b, 'err)`

     It is strictly equivalent to `m->Result.map(x => n)`
     */

  let let_: (result(_), _) => result(_) = Result.map;
};

module Ft = {
  /* [let%Ft x = m; n] means:
     - [m] has type `Promise.t('a)`
     - [x] is an identifier of type `'a`
     - [n] has type `Promise.t('b)`
     - the whole expression has then type `Promise.t('b)`

     It is strictly equivalent to `m->Promise.flatMap(x => n)`
     */

  let let_: (Promise.t(_), _) => Promise.t(_) = Promise.flatMap;
};

module FtMap = {
  /* [let%FtMap x = m; n] means:
     - [m] has type `Promise.t('a)`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Promise.t('b)`

     It is strictly equivalent to `m->Promise.map(x => n)`
     */

  let let_: (Promise.t(_), _) => Promise.t(_) = Promise.map;
};

module FRes = {
  /* [let%FRes x = m; n] means:
     - [m] has type `Promise.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `Promise.t(Result.t('b, 'err))`
     - the whole expression has then type `Promise.t(Result.t('b, 'err))`

     It is strictly equivalent to `m->Promise.flatMapOk(x => n)`
     */

  let let_: (Promise.t(_), _) => Promise.t(_) = Promise.flatMapOk;
};

module FResMap = {
  /* [let%FResMap x = m; n] means:
     - [m] has type `Promise.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Promise.t(Result.t('b, 'err))`

     It is strictly equivalent to `m->Promise.mapOk(x => n)`
     */

  let let_: (Promise.t(_), _) => Promise.t(_) = Promise.mapOk;
};

module FlatRes = {
  /* [let%FlatRes x = m; n] means:
     - [m] has type `Promise.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `Result.t('b, 'err)`
     - the whole expression has then type `result('b, 'err)`

     It is strictly equivalent to `m->Promise.flatMapOk(x => n->Promise.value)`
     */

  let let_: (Promise.t(_), _) => Promise.t(_) =
    (fut, fn) => fut->Promise.flatMapOk(v => v->fn->Promise.value);
};
