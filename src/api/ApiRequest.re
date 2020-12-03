type t('value) =
  | NotAsked
  | Loading
  | Done(Belt.Result.t('value, string));

let getDone = request =>
  switch (request) {
  | Done(result) => Some(result)
  | _ => None
  };

let getDoneOk = request =>
  switch (request) {
  | Done(Ok(value)) => Some(value)
  | _ => None
  };

let getOkWithDefault = (request, def) =>
  switch (request) {
  | Done(Ok(value)) => value
  | _ => def
  };

let map = (request, f) =>
  switch (request) {
  | Done(result) => f(result)
  | _ => ()
  };

let getOk = (request, f) =>
  switch (request) {
  | Done(Ok(value)) => f(value)
  | _ => ()
  };

let mapOrLoad = (req, f) =>
  switch (req) {
  | Done(res) => f(res)
  | NotAsked
  | Loading => <LoadingView />
  };

let isNotAsked = request => request == NotAsked;

let isLoading = request => request == Loading;

let isDone = request => request->getDone->Belt.Option.isSome;

let logError = (r, addLog, origin) =>
  r->Future.tapError(msg =>
    addLog(Logs.{kind: Logs.Error, msg, origin, timestamp: Js.Date.now()})
  );

let logOk = (r, addLog, origin, makeMsg) =>
  r->Future.tapOk(r => {
    addLog(
      Logs.{
        kind: Logs.Info,
        msg: makeMsg(r),
        origin,
        timestamp: Js.Date.now(),
      },
    )
  });

let conditionToLoad = (request, isMounted) => {
  let requestNotAskedAndMonted = request->isNotAsked && isMounted;
  let requestDoneButReloadOnMont = request->isDone && !isMounted;
  (requestNotAskedAndMonted || requestDoneButReloadOnMont, isMounted);
};

let useGetter = (~toast=true, ~get, ~kind, ~setRequest, ()) => {
  let addLog = LogsContext.useAdd();
  let config = ConfigContext.useConfig();

  let get = (~loading=true, input) => {
    loading ? setRequest(_ => Loading) : ();
    get(~config, input)
    ->logError(addLog(toast), kind)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  get;
};

let useLoader = (~get, ~kind, ~requestState as (request, setRequest)) => {
  let getRequest = useGetter(~get, ~kind, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let (shouldReload, loading) = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest(~loading, ());
      };

      None;
    },
    (isMounted, request, setRequest),
  );

  request;
};

let useLoader1 = (~get, ~kind, ~requestState as (request, setRequest), arg1) => {
  let getRequest = useGetter(~get, ~kind, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect4(
    () => {
      let (shouldReload, loading) = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest(~loading, arg1);
      };

      None;
    },
    (isMounted, arg1, request, setRequest),
  );

  request;
};

let useLoader2 =
    (~get, ~kind, ~requestState as (request, setRequest), arg1, arg2) => {
  let getRequest = useGetter(~get, ~kind, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect5(
    () => {
      let (shouldReload, loading) = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest(~loading, (arg1, arg2));
      };

      None;
    },
    (isMounted, arg1, arg2, request, setRequest),
  );

  request;
};

let useSetter = (~toast=true, ~sideEffect=?, ~set, ~kind, ()) => {
  let addLog = LogsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = input => {
    setRequest(_ => Loading);
    set(~config, input)
    ->logError(addLog(toast), kind)
    ->Future.tap(result => {setRequest(_ => Done(result))})
    ->Future.tapOk(sideEffect->Belt.Option.getWithDefault(_ => ()));
  };

  (request, sendRequest);
};
