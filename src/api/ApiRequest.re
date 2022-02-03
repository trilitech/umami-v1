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

type timestamp = float;

type cacheValidity =
  | Expired
  | ValidSince(float);

let initCache = () => ValidSince(Js.Date.now());

type t('value) =
  | NotAsked
  | Loading(option('value))
  | Done(Result.t('value, Errors.t), cacheValidity);

type setRequest('value) = (t('value) => t('value)) => unit;

type requestState('value) = (t('value), setRequest('value));

let getDone = request =>
  switch (request) {
  | Done(result, _) => Some(result)
  | Loading(Some(value)) => Some(Ok(value))
  | _ => None
  };

let getDoneOk = request =>
  switch (request) {
  | Done(Ok(value), _)
  | Loading(Some(value)) => Some(value)
  | _ => None
  };

let getWithDefault = (request, def) =>
  switch (request) {
  | Done(Ok(value), _)
  | Loading(Some(value)) => value
  | _ => def
  };

let iterDone = (request, f) =>
  switch (request) {
  | Done(result, _) => f(result)
  | Loading(Some(value)) => f(Ok(value))
  | _ => ()
  };

let map = (request, f) =>
  switch (request) {
  | Done(Ok(value), t) => Done(Ok(f(value)), t)
  | Loading(Some(value)) => Loading(Some(f(value)))
  | r => r
  };

let mapWithDefault = (request, def, f) =>
  switch (request) {
  | Done(Ok(value), _)
  | Loading(Some(value)) => f(value)
  | _ => def
  };

let mapOrLoad = (req, f) =>
  switch (req) {
  | Done(Ok(data), _)
  | Loading(Some(data)) => f(data)
  | Done(Error(_), _)
  | NotAsked
  | Loading(None) => <LoadingView />
  };

let mapOrEmpty = (req, f) =>
  switch (req) {
  | Done(Ok(data), _)
  | Loading(Some(data)) => f(data)
  | Done(Error(_), _)
  | NotAsked
  | Loading(None) => React.null
  };

let isNotAsked = request => request == NotAsked;

let isLoading = request =>
  switch (request) {
  | Loading(_) => true
  | _ => false
  };

let isError = request =>
  switch (request) {
  | Done(Error(_), _) => true
  | _ => false
  };

let isDone = request => request->getDone->Option.isSome;
let doneOk = v => Done(Ok(v), ValidSince(Js.Date.now()));
let doneErr = e => Done(Error(e), ValidSince(Js.Date.now()));

let isDoneOk = request => request->getDoneOk->Option.isSome;

let delay = 30. *. 1000.; // 30sec

let isExpired = request =>
  switch (request) {
  | Done(_, Expired) => true
  | Done(_, ValidSince(timestamp)) => Js.Date.now() -. timestamp > delay
  | _ => false
  };

let isForceExpired = request =>
  switch (request) {
  | Done(_, Expired) => true
  | _ => false
  };

let logError = (r, addLog, ~keep=_ => true, origin) =>
  r->Promise.tapError(e => {e->keep ? addLog(Logs.error(~origin, e)) : ()});

let logOk = (r, addLog, origin, makeMsg) =>
  r->Promise.tapOk(r => {addLog(Logs.info(~origin, makeMsg(r)))});

let updateToLoadingState = request =>
  switch (request) {
  | Done(Ok(data), _) => Loading(Some(data))
  | Loading(Some(data)) => Loading(Some(data))
  | _ => Loading(None)
  };

let expireCache = request =>
  switch (request) {
  | Done(result, _) => Done(result, Expired)
  | other => other
  };

let conditionToLoad = (request, isMounted) => {
  let requestNotAskedAndMounted = request->isNotAsked && isMounted;
  let requestDoneButReloadOnMount = request->isDone && !isMounted;
  let requestExpired = request->isExpired;
  requestNotAskedAndMounted || requestDoneButReloadOnMount || requestExpired;
};

module type DEPS = {
  let useAddLog: (unit, bool, Umami.Logs.t) => unit;
  let useConfig: unit => ConfigContext.env;
  let useRetryNetwork: unit => ConfigContext.retryNetworkType;
};

module Make = (D: DEPS) => {
  let useGetter = (~toast=true, ~get, ~kind, ~setRequest, ~keepError=?, ()) => {
    let config = D.useConfig();
    let addLog = D.useAddLog();
    let retryNetwork = D.useRetryNetwork();

    let get = input => {
      setRequest(updateToLoadingState);

      get(~config, input)
      ->logError(addLog(toast), kind, ~keep=?keepError)
      ->Promise.tapError(
          fun
          | ReTaquitoError.NodeRequestFailed =>
            retryNetwork(~onlyOn=Network.Online, ())
          | _ => (),
        )
      ->Promise.tap(result =>
          setRequest(_ => Done(result, ValidSince(Js.Date.now())))
        );
    };

    get;
  };

  let useLoader =
      (
        ~get,
        ~condition=?,
        ~keepError=?,
        ~kind,
        ~requestState as (request, setRequest),
        input,
      ) => {
    let getRequest = useGetter(~get, ~kind, ~setRequest, ~keepError?, ());

    let isMounted = ReactUtils.useIsMounted();

    React.useEffect4(
      () => {
        let shouldReload = conditionToLoad(request, isMounted);
        let condition = condition->Option.mapWithDefault(true, f => input->f);

        if (shouldReload && condition) {
          getRequest(input)->ignore;
        };

        None;
      },
      (isMounted, request, input, setRequest),
    );

    request;
  };

  let useSetter =
      (~logOk=?, ~toast=true, ~sideEffect=?, ~set, ~kind, ~keepError=?, ()) => {
    let addLog = D.useAddLog();
    let (request, setRequest) = React.useState(_ => NotAsked);
    let config = ConfigContext.useContent();

    let sendRequest = input => {
      setRequest(_ => Loading(None));
      set(~config, input)
      ->Promise.tapOk(v =>
          logOk->Option.iter(f =>
            addLog(true, Logs.info(~origin=kind, f(v)))
          )
        )
      ->logError(addLog(toast), ~keep=?keepError, kind)
      ->Promise.tap(result => {
          setRequest(_ => Done(result, Js.Date.now()->ValidSince))
        })
      ->Promise.tapOk(sideEffect->Option.getWithDefault(_ => ()));
    };

    (request, sendRequest);
  };

  let toString = {
    let cacheToString =
      fun
      | Expired => "Expired"
      | ValidSince(_) => "ValidSince";
    fun
    | NotAsked => "NotAsked"
    | Loading(None) => "Loading(None)"
    | Loading(Some(_)) => "Loading(Some)"
    | Done(Ok(_), c) => "Done(Ok, " ++ cacheToString(c) ++ ")"
    | Done(Error(_), c) => "Done(Error, " ++ cacheToString(c) ++ ")";
  };

  let eqCache = (c1, c2) =>
    switch (c1, c2) {
    | (Expired, Expired) => true
    | (ValidSince(_), ValidSince(_)) => true
    | _ => false
    };

  let eq = (~eq=(==), a1, a2) =>
    switch (a1, a2) {
    | (Done(Ok(v1), c1), Done(Ok(v2), c2)) =>
      eq(v1, v2) && eqCache(c1, c2)
    | (Done(Error(_), c1), Done(Error(_), c2)) => eqCache(c1, c2)
    | (NotAsked, NotAsked) => true
    | (Loading(None), Loading(None)) => true
    | (Loading(Some(v1)), Loading(Some(v2))) => eq(v1, v2)
    | _ => false
    };
};

module Impl = {
  let useAddLog = LogsContext.useAdd;
  let useConfig = ConfigContext.useContent;
  let useRetryNetwork = ConfigContext.useRetryNetwork;
};

include Make(Impl);
