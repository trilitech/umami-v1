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

let fromOptionWithDefault = (option, ~default) =>
  Future.value(
    switch (option) {
    | Some(value) => Ok(value)
    | None => Ok(default)
    },
  );

let all = array =>
  array
  ->List.fromArray
  ->Future.all
  ->Future.map(results => Ok(results->List.toArray));

let fromCallback = (f, mapError) =>
  Future.make(resolve =>
    {
      (e, v) =>
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e->mapError)->resolve
        | None => Ok(v)->resolve
        };
    }
    ->f
  );

let fromUnitCallback = (f, mapError) =>
  Future.make(resolve =>
    {
      e =>
        switch (Js.Nullable.toOption(e)) {
        | Some(e) => Error(e->mapError)->resolve
        | None => Ok()->resolve
        };
    }
    ->f
  );
