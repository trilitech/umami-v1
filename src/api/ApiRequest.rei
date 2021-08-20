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

type t('value, 'error) =
  /* The ressource fetching has never been triggered */
  | NotAsked
  /* The ressource fetch has started. The [option('value)] is the state of the
     last fetch */
  | Loading(option('value))
  /* The ressource has been fetched or fell into an error. */
  | Done(Result.t('value, 'error), cacheValidity);

/* Interface of a function able to update a ressource state of type [t] */
type setRequest('value, 'error) =
  (t('value, 'error) => t('value, 'error)) => unit;

/* A ressource with its state and its update function */
type requestState('value, 'error) = (
  t('value, 'error),
  setRequest('value, 'error),
);

/* Returns an optional result which value is [Some] if the ressource has
   been fetched */
let getDone: t('a, 'b) => option(Belt.Result.t('a, 'b));

/* Returns an optional value which value is [Some] if the ressource has
   been fetched with no error. */
let getDoneOk: t('a, 'b) => option('a);

/* Returns the value of the ressource if it's been fetched. If it has not been
   fetched or resulted in an error, it returns the second parameter. */
let getWithDefault: (t('a, 'b), 'a) => 'a;

/* Applies a side effect to the given ressource value if it has been fetched */
let iterDone: (t('a, 'b), Belt.Result.t('a, 'b) => unit) => unit;

/* Maps the value of the ressource  */
let map: (t('a, 'b), 'a => 'a) => t('a, 'b);

/* Maps the value of the ressource if it has been fetched. If it has not been
   fetched or resulted in an error, it returns the second parameter. */
let mapWithDefault: (t('a, 'b), 'c, 'a => 'c) => 'c;

/* Returns a loading element if the ressource has not been fetched or resulted
   in an error. Otherwise, it maps on it */
let mapOrLoad: (t('a, 'b), 'a => React.element) => React.element;

/* Returns a null element if the ressource failed or is not fetched and maps on
   it otherwise*/
let mapOrEmpty: (t('a, 'b), 'a => React.element) => React.element;

/* Returns [true] if the ressource fetching has started */
let isNotAsked: t('a, 'b) => bool;

/* Returns [true] if the ressource fetching has started and is not over */
let isLoading: t('a, 'b) => bool;

/* Returns [true] if the ressource is fetched */
let isDone: t('a, 'b) => bool;

/* Returns [true] if the ressource is fetched without error. */
let isDoneOk: t('a, 'b) => bool;

/* Returns [true] if the difference between last fetching and current time
   is higher than the delay */
let isExpired: t('a, 'b) => bool;

/* Produces an error log side effect if the given futur computes as an error.
   [keep] aims at filtering some errors based on the error value. No error is
   filtered by default */
let logError:
  (
    Let.future('a),
    Logs.t => unit,
    ~keep: Errors.t => bool=?,
    ~toString: Errors.t => Js.String.t=?,
    Logs.origin
  ) =>
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
let updateToLoadingState: t('a, 'b) => t('a, 'c);

/* Expire the cache so the ressource must be refreshed */
let expireCache: t('a, 'b) => t('a, 'b);

/* Checks if a ressource should be reloaded based on its current state */
let conditionToLoad: (t('a, 'b), bool) => bool;

/* Builds a future hook using an asynchronous function.
   The fetching is called manually.
   Basic block to build [useLoader] and [useSetter] */
let useGetter:
  (
    ~toast: bool=?,
    ~errorToString: Errors.t => string=?,
    ~get: (~config: ConfigFile.t, 'a) => Let.future('response),
    ~kind: Logs.origin,
    ~setRequest: (t('response, 'c) => t('response, Errors.t)) => unit,
    unit,
    'a
  ) =>
  Let.future('response);

/* Builds an auto-reloaded ressource from an asynchronous function */
let useLoader:
  (
    ~get: (~config: ConfigFile.t, 'input) => Let.future('value),
    ~condition: 'input => bool=?,
    ~kind: Logs.origin,
    ~errorToString: Errors.t => string=?,
    ~requestState: requestState('value, Errors.t),
    'input
  ) =>
  t('value, Errors.t);

/* Builds a ressource that represents the modification of a distant value
     defined through the [set] function.
   */
let useSetter:
  (
    ~toast: bool=?,
    ~sideEffect: 'a => unit=?,
    ~set: (~config: ConfigFile.t, 'c) => Let.future('a),
    ~kind: Logs.origin,
    ~keepError: Errors.t => bool=?,
    ~errorToString: Errors.t => Js.String.t=?,
    unit
  ) =>
  (t('a, Errors.t), 'c => Let.future('a));
