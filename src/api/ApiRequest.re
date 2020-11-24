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

let handleLog = (r, addLog, kind, origin) =>
  r->Future.tapError(msg =>
    addLog(Logs.{kind, msg, origin, timestamp: Js.Date.now()})
  );

let useLoader = (get, kind, ()) => {
  let (request, setRequest) = React.useState(_ => NotAsked);
  let addLog = ErrorsContext.useAdd();
  let config = ConfigContext.useConfig();

  React.useEffect1(
    () => {
      setRequest(_ => Loading);

      get(~config)
      ->handleLog(addLog, Logs.Error, kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    [|setRequest|],
  );

  request;
};

let useLoader1 = (get, kind, arg1) => {
  let addLog = ErrorsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect3(
    () => {
      setRequest(_ => Loading);

      get(~config, arg1)
      ->handleLog(addLog, Logs.Error, kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (config, arg1, setRequest),
  );

  request;
};

let useLoader2 = (get, kind, arg1, arg2) => {
  let addLog = ErrorsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  React.useEffect4(
    () => {
      setRequest(_ => Loading);

      get(~config, arg1, arg2)
      ->handleLog(addLog, Logs.Error, kind)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (config, arg1, arg2, setRequest),
  );

  request;
};

let useSetter = (set, kind, ()) => {
  let addLog = ErrorsContext.useAdd();
  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let sendRequest = input => {
    setRequest(_ => Loading);
    set(~config, input)
    ->handleLog(addLog, Logs.Error, kind)
    ->Future.tap(result => {setRequest(_ => Done(result))});
  };

  (request, sendRequest);
};

let useGetter = (get, kind) => {
  let addLog = ErrorsContext.useAdd();
  let (request, setRequest) = React.useState(_ => Loading);
  let config = ConfigContext.useConfig();

  let get = (~loading=true, input) => {
    loading ? setRequest(_ => Loading) : ();
    get(~config, input)
    ->handleLog(addLog, Logs.Error, kind)
    ->Future.get(result => setRequest(_ => {Done(result)}));
  };

  (get, request);
};
