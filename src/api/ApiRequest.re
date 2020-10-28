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

let map = (request, f) =>
  switch (request) {
  | Done(result) => f(result)
  | _ => ()
  };

let mapOk = (request, f) =>
  switch (request) {
  | Done(Ok(value)) => f(value)
  | _ => ()
  };

let isLoading = request => request == Loading;
