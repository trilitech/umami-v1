let getOk = (future, sink) =>
  future->Future.get(result => result->ResultEx.getOk(sink));

let getError = (future, sink) =>
  future->Future.get(result => result->ResultEx.getError(sink));