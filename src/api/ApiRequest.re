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

open UmamiCommon;

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

let isDoneOk = request => request->getDoneOk->Option.isSome;

let delay = 30. *. 1000.; // 30sec

let isExpired = request =>
  switch (request) {
  | Done(_, Expired) => true
  | Done(_, ValidSince(timestamp)) => Js.Date.now() -. timestamp > delay
  | _ => false
  };

let logError = (r, addLog, ~keep=_ => true, origin) =>
  r->Future.tapError(e => {e->keep ? addLog(Logs.error(~origin, e)) : ()});

let logOk = (r, addLog, origin, makeMsg) =>
  r->Future.tapOk(r => {addLog(Logs.info(~origin, makeMsg(r)))});

let updateToLoadingState = request =>
  switch (request) {
  | Done(Ok(data), _) => Loading(Some(data))
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

let useGetter = (~toast=true, ~get, ~kind, ~setRequest, ~keepError=?, ()) => {
  let addLog = LogsContext.useAdd();
  let config = ConfigContext.useContent();
  let retryNetwork = ConfigContext.useRetryNetwork();

  let get = input => {
    setRequest(updateToLoadingState);

    get(~config, input)
    ->logError(addLog(toast), kind, ~keep=?keepError)
    ->Future.tapError(
        fun
        | ReTaquitoError.NodeRequestFailed =>
          retryNetwork(~onlyOn=Network.Online, ())
        | _ => (),
      )
    ->Future.tap(result =>
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

  let isMounted = ReactUtils.useIsMonted();
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
  let addLog = LogsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useContent();

  let sendRequest = input => {
    setRequest(_ => Loading(None));
    set(~config, input)
    ->Future.tapOk(v =>
        logOk->Lib.Option.iter(f =>
          addLog(true, Logs.info(~origin=kind, f(v)))
        )
      )
    ->logError(addLog(toast), ~keep=?keepError, kind)
    ->Future.tap(result => {
        setRequest(_ => Done(result, Js.Date.now()->ValidSince))
      })
    ->Future.tapOk(sideEffect->Option.getWithDefault(_ => ()));
  };

  (request, sendRequest);
};
