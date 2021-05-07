type t('value, 'error) =
  | NotAsked
  | Loading(option('value))
  | Done(Result.t('value, 'error), float);

type setRequest('value, 'error) =
  (t('value, 'error) => t('value, 'error)) => unit;

type requestState('value, 'error) = (
  t('value, 'error),
  setRequest('value, 'error),
);

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

let isDone = request => request->getDone->Option.isSome;

let delay = 30. *. 1000.; // 30sec

let isExpired = request =>
  switch (request) {
  | Done(_, timestamp) => Js.Date.now() -. timestamp > delay
  | _ => false
  };

let logError = (r, addLog, ~keep=_ => true, ~toString=Js.String.make, origin) =>
  r->Future.tapError(msg =>
    msg->keep
      ? addLog(
          Logs.{
            kind: Logs.Error,
            msg: msg->toString,
            origin,
            timestamp: Js.Date.now(),
          },
        )
      : ()
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

let updateToLoadingState = request =>
  switch (request) {
  | Done(Ok(data), _) => Loading(Some(data))
  | _ => Loading(None)
  };

let updateToResetState = request =>
  switch (request) {
  | Done(result, _) => Done(result, 0.0)
  | other => other
  };

let conditionToLoad = (request, isMounted) => {
  let requestNotAskedAndMounted = request->isNotAsked && isMounted;
  let requestDoneButReloadOnMount = request->isDone && !isMounted;
  let requestExpired = request->isExpired;
  requestNotAskedAndMounted || requestDoneButReloadOnMount || requestExpired;
};

let useGetter =
    (
      ~toast=true,
      ~errorToString=?,
      ~get:
         (~settings: TezosClient.AppSettings.t, 'input) =>
         Future.t(Belt.Result.t('response, string)),
      ~kind,
      ~setRequest,
      (),
    )
    : ('input => Future.t(Belt.Result.t('response, string))) => {
  let addLog = LogsContext.useAdd();
  let settings = SdkContext.useSettings();

  let get = input => {
    setRequest(updateToLoadingState);

    get(~settings, input)
    ->logError(addLog(toast), ~toString=?errorToString, kind)
    ->Future.tap(result => setRequest(_ => Done(result, Js.Date.now())));
  };

  get;
};

let useLoader =
    (
      ~get,
      ~kind,
      ~errorToString=?,
      ~requestState as (request, setRequest): requestState('value, 'error),
      (),
    ) => {
  let getRequest = useGetter(~get, ~kind, ~errorToString?, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      let shouldReload = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest()->ignore;
      };

      None;
    },
    (isMounted, request, setRequest),
  );

  request;
};

let useLoader1 =
    (
      ~get,
      ~kind,
      ~errorToString=?,
      ~requestState as (request, setRequest): requestState('value, 'error),
      arg1,
    ) => {
  let getRequest = useGetter(~get, ~kind, ~errorToString?, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect4(
    () => {
      let shouldReload = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest(arg1)->ignore;
      };

      None;
    },
    (isMounted, arg1, request, setRequest),
  );

  request;
};

let useLoader2 =
    (
      ~get,
      ~kind,
      ~errorToString=?,
      ~requestState as (request, setRequest): requestState('value, 'error),
      arg1,
      arg2,
    ) => {
  let getRequest = useGetter(~get, ~kind, ~errorToString?, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect5(
    () => {
      let shouldReload = conditionToLoad(request, isMounted);
      if (shouldReload) {
        getRequest((arg1, arg2))->ignore;
      };

      None;
    },
    (isMounted, arg1, arg2, request, setRequest),
  );

  request;
};

let useSetter =
    (
      ~toast=true,
      ~sideEffect=?,
      ~set: (~settings: _, _) => Future.t(Result.t(_, 'b)),
      ~kind,
      ~keepError=?,
      ~errorToString=?,
      (),
    ) => {
  let addLog = LogsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let settings = SdkContext.useSettings();

  let sendRequest = input => {
    setRequest(_ => Loading(None));
    set(~settings, input)
    ->logError(
        addLog(toast),
        ~keep=?keepError,
        ~toString=?errorToString,
        kind,
      )
    ->Future.tap(result => {setRequest(_ => Done(result, Js.Date.now()))})
    ->Future.tapOk(sideEffect->Option.getWithDefault(_ => ()));
  };

  (request, sendRequest);
};
