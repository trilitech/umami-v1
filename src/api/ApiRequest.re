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

let isLoading = request => request == Loading;

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

let useLoader = (get, kind, ()) => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let addToast = LogsContext.useToast();
  let config = ConfigContext.useConfig();

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      get(~config)
      ->logError(addToast, kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    [|setRequest|],
  );

  request;
};

let useLoader1 = (get, kind, arg1) => {
  let addToast = LogsContext.useToast();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect3(
    () => {
      setRequest(_ => Loading);

      get(~config, arg1)
      ->logError(addToast, kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (config, arg1, setRequest),
  );

  request;
};

let useLoader2 = (get, kind, arg1, arg2) => {
  let addLog = LogsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect4(
    () => {
      setRequest(_ => Loading);

      get(~config, arg1, arg2)
      ->logError(addLog(true), kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (config, arg1, arg2, setRequest),
  );

  request;
};

let useStoreLoader = (get, kind, request, setRequest) => {
  let addLog = LogsContext.useAdd();
  let config = ConfigContext.useConfig();

  React.useEffect3(
    () => {
      if (request == NotAsked) {
        setRequest(_ => Loading);

        get(~config)
        ->logError(addLog(true), kind)
        ->Future.get(result => setRequest(_ => Done(result)));
      };

      None;
    },
    (config, request, setRequest),
  );
};

let useStoreLoader1 = (get, kind, request, setRequest, arg1) => {
  let addLog = LogsContext.useAdd();
  let config = ConfigContext.useConfig();

  React.useEffect4(
    () => {
      if (request == NotAsked) {
        setRequest(_ => Loading);

        get(~config, arg1)
        ->logError(addLog(true), kind)
        ->Future.get(result => setRequest(_ => Done(result)));
      };

      None;
    },
    (config, arg1, request, setRequest),
  );
};

let useStoreLoader2 = (get, kind, request, setRequest, arg1, arg2) => {
  let addLog = LogsContext.useAdd();
  let config = ConfigContext.useConfig();

  React.useEffect5(
    () => {
      if (request == NotAsked) {
        setRequest(_ => Loading);

        get(~config, arg1, arg2)
        ->logError(addLog(true), kind)
        ->Future.get(result => setRequest(_ => Done(result)));
      };

      None;
    },
    (config, arg1, arg2, request, setRequest),
  );
};

let useSetter = (~toast=true, ~sideEffect=?, set, kind, ()) => {
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

let useGetter = (~toast=true, get, kind) => {
  let addLog = LogsContext.useAdd();
  let (request, setRequest) = React.useState(_ => Loading);
  let config = ConfigContext.useConfig();

  let get = (~loading=true, input) => {
    loading ? setRequest(_ => Loading) : ();
    get(~config, input)
    ->logError(addLog(toast), kind)
    ->Future.get(result => setRequest(_ => {Done(result)}));
  };

  (get, request);
};

let useStoreGetter = (~toast=true, get, kind, setRequest) => {
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
