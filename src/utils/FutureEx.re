let getOk = (future, sink) =>
  future->Future.get(result => result->ResultEx.getOk(sink));

let getError = (future, sink) =>
  future->Future.get(result => result->ResultEx.getError(sink));

let fromOption = (option, ~error) =>
  Future.value(
    switch (option) {
    | Some(value) => Ok(value)
    | None => Error(error)
    },
  );

let all = array =>
  array->List.fromArray->Future.all->Future.map(results => Ok(results));
