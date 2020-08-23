let getOk = (future, sink) =>
  future->Future.get(result => result->ResultEx.getOk(sink));