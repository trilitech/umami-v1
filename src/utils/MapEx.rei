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

/** Real OCaml maps functor */

module type S = {
  include (module type of Belt.Map.Dict);

  module Key: Id.Comparable;
  type key = Key.t;
  type id = Key.identity;
  type map('value) = t(key, 'value, id);

  let kcmp: cmp(key, id);
  let cmp: (t(key, 'a, id), t(key, 'a, id), ~vcmp: ('a, 'a) => int) => int;
  let has: (t(key, 'a, id), key) => bool;
  let eq: (t(key, 'a, id), t(key, 'a, id), ~veq: ('a, 'a) => bool) => bool;
  let fromArray: array((key, 'a)) => t(key, 'a, id);
  let get: (t(key, 'a, id), key) => option('a);
  let getWithDefault: (t(key, 'a, id), key, 'a) => 'a;
  let remove: (t(key, 'a, id), key) => t(key, 'a, id);
  let removeMany: (t(key, 'a, id), array(key)) => t(key, 'a, id);
  let set: (t(key, 'a, id), key, 'a) => t(key, 'a, id);
  let update:
    (t(key, 'a, id), key, option('a) => option('a)) => t(key, 'a, id);
  let merge:
    (
      t(key, 'a, id),
      t(key, 'b, id),
      (key, option('a), option('b)) => option('c)
    ) =>
    t(key, 'c, id);
  let mergeMany: (t(key, 'a, id), array((key, 'a))) => t(key, 'a, id);
  let split:
    (t(key, 'a, id), key) =>
    ((t(key, 'a, id), t(key, 'a, id)), option('a));
};

module Make: (Key: Id.Comparable) => S with module Key := Key;
