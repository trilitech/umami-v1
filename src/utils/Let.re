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

  let let_ = Result.flatMap;
};

module ResMap = {
  /* [let%ResMap x = m; n] means:
     - [m] has type `Result.t('a, 'err)`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Result.t('b, 'err)`

     It is strictly equivalent to `m->Result.map(x => n)`
     */

  let let_ = Result.map;
};

module Ft = {
  /* [let%Ft x = m; n] means:
     - [m] has type `Future.t('a)`
     - [x] is an identifier of type `'a`
     - [n] has type `Future.t('b)`
     - the whole expression has then type `Future.t('b)`

     It is strictly equivalent to `m->Future.flatMap(x => n)`
     */

  let let_ = Future.flatMap;
};

module FtMap = {
  /* [let%FtMap x = m; n] means:
     - [m] has type `Future.t('a)`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Future.t('b)`

     It is strictly equivalent to `m->Future.map(x => n)`
     */

  let let_ = Future.map;
};

module FRes = {
  /* [let%FRes x = m; n] means:
     - [m] has type `Future.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `Future.t(Result.t('b, 'err))`
     - the whole expression has then type `Future.t(Result.t('b, 'err))`

     It is strictly equivalent to `m->Future.flatMapOk(x => n)`
     */

  let let_ = Future.flatMapOk;
};

module FResMap = {
  /* [let%FResMap x = m; n] means:
     - [m] has type `Future.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `'b`
     - the whole expression has then type `Future.t(Result.t('b, 'err))`

     It is strictly equivalent to `m->Future.mapOk(x => n)`
     */

  let let_ = Future.mapOk;
};

module FlatRes = {
  /* [let%FlatRes x = m; n] means:
     - [m] has type `Future.t(Result.t('a, 'err))`
     - [x] is an identifier of type `'a`
     - [n] has type `Result.t('b, 'err)`
     - the whole expression has then type `result('b, 'err)`

     It is strictly equivalent to `m->Future.flatMapOk(x => n->Future.value)`
     */

  let let_ = (fut, fn) => fut->Future.flatMapOk(v => v->fn->Future.value);
};
