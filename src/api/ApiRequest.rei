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

/*
    This module aims at describing state of long running ressource fetching from
    external locations: network, disk, etc. If the ressource can be fetched
    multiple times, it is possible to change from [Done] to [Loading].

    The ressource is represented as type [t]. This type is composed from several
    variants.
 */

type timestamp = float;

/* A ressource is considered cached since a given date.
   A cache can be invalidated:
   - [cacheValidity] is set [Expired]
   - [cacheValidity] is set [ValidSince(ts)] with [ts] being "old enough".
     "old enough" is defined by the [isExpired] function
   If the cache is invalid, the ressource should be fetched again. */
type cacheValidity =
  | Expired
  | ValidSince(timestamp);

type t('value) =
  /* The ressource fetching has never been triggered */
  | NotAsked
  /* The ressource fetch has started. The [option('value)] is the state of the
     last fetch */
  | Loading(option('value))
  /* The ressource has been fetched or fell into an error. */
  | Done(Result.t('value, Errors.t), cacheValidity);

/* Interface of a function able to update a ressource state of type [t] */
type setRequest('value) = (t('value) => t('value)) => unit;

/* A ressource with its state and its update function */
type requestState('value) = (t('value), setRequest('value));

/* Returns an optional result which value is [Some] if the ressource has
   been fetched */
let getDone: t('a) => option(Let.result('a));

/* Returns an optional value which value is [Some] if the ressource has
   been fetched with no error. */
let getDoneOk: t('a) => option('a);

/* Returns the value of the ressource if it's been fetched. If it has not been
   fetched or resulted in an error, it returns the second parameter. */
let getWithDefault: (t('a), 'a) => 'a;

/* Applies a side effect to the given ressource value if it has been fetched */
let iterDone: (t('a), Let.result('a) => unit) => unit;

/* Maps the value of the ressource  */
let map: (t('a), 'a => 'a) => t('a);

/* Maps the value of the ressource if it has been fetched. If it has not been
   fetched or resulted in an error, it returns the second parameter. */
let mapWithDefault: (t('a), 'c, 'a => 'c) => 'c;

/* Returns a loading element if the ressource has not been fetched or resulted
   in an error. Otherwise, it maps on it */
let mapOrLoad: (t('a), 'a => React.element) => React.element;

/* Returns a null element if the ressource failed or is not fetched and maps on
   it otherwise*/
let mapOrEmpty: (t('a), 'a => React.element) => React.element;

/* Returns [true] if the ressource fetching has started */
let isNotAsked: t('a) => bool;

/* Returns [true] if the ressource fetching has started and is not over */
let isLoading: t('a) => bool;

/* Returns [true] if the ressource is fetched */
let isDone: t('a) => bool;

/* Returns [true] if the ressource is fetched without error. */
let isDoneOk: t('a) => bool;

/* Returns [true] if the difference between last fetching and current time
   is higher than the delay */
let isExpired: t('a) => bool;

/* Produces an error log side effect if the given futur computes as an error.
   [keep] aims at filtering some errors based on the error value. No error is
   filtered by default */
let logError:
  (Let.future('a), Logs.t => unit, ~keep: Errors.t => bool=?, Logs.origin) =>
  Let.future('a);

/* Same as [logError] but for [Ok] result values */
let logOk:
  (
    Future.t(Belt.Result.t('a, 'b)),
    Logs.t => 'c,
    Logs.origin,
    'a => string
  ) =>
  Future.t(Belt.Result.t('a, 'b));

/* Sets the ressource status as [Loading] */
let updateToLoadingState: t('a) => t('a);

/* Expire the cache so the ressource must be refreshed */
let expireCache: t('a) => t('a);

/* Checks if a ressource should be reloaded based on its current state */
let conditionToLoad: (t('a), bool) => bool;

/* Builds a future hook using an asynchronous function.
   The fetching is called manually.
   Basic block to build [useLoader] and [useSetter] */
let useGetter:
  (
    ~toast: bool=?,
    ~get: (~config: ConfigContext.env, 'a) => Let.future('response),
    ~kind: Logs.origin,
    ~setRequest: (t('response) => t('response)) => unit,
    ~keepError: Errors.t => bool=?,
    unit,
    'a
  ) =>
  Let.future('response);

/* Builds an auto-reloaded ressource from an asynchronous function */
let useLoader:
  (
    ~get: (~config: ConfigContext.env, 'input) => Let.future('value),
    ~condition: 'input => bool=?,
    ~keepError: Errors.t => bool=?,
    ~kind: Logs.origin,
    ~requestState: requestState('value),
    'input
  ) =>
  t('value);

/* Builds a ressource that represents the modification of a distant value
     defined through the [set] function.
   */
let useSetter:
  (
    ~logOk: 'a => string=?,
    ~toast: bool=?,
    ~sideEffect: 'a => unit=?,
    ~set: (~config: ConfigContext.env, 'c) => Let.future('a),
    ~kind: Logs.origin,
    ~keepError: Errors.t => bool=?,
    unit
  ) =>
  (t('a), 'c => Let.future('a));
