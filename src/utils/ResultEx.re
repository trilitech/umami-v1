let getOk = (result, sink) =>
  switch (result) {
  | Ok(value) => sink(value)
  | Error(_) => ()
  };

let getError = (result, sink) =>
  switch (result) {
  | Ok(_) => ()
  | Error(value) => sink(value)
  };

let fromOption = (v, error) =>
  switch (v) {
  | Some(v) => Ok(v)
  | None => error
  };

let flatMap2 = (r1, r2, f) =>
  switch (r1, r2) {
  | (Ok(v1), Ok(v2)) => f(v1, v2)
  | (Error(err), _)
  | (_, Error(err)) => Error(err)
  };

let map2 = (r1, r2, f) => flatMap2(r1, r2, (v1, v2) => Ok(f(v1, v2)));

let collect = (type err, l: list(result(_, err))) => {
  // let's quit the reduce as soon as e have an error
  exception Fail(err);
  try(
    l
    // This uses reduce + reverse to always catch the first error in the list
    ->List.reduce([], (l, v) =>
        switch (v) {
        | Ok(v) => [v, ...l]
        | Error(e) => raise(Fail(e))
        }
      )
    ->List.reverse
    ->Ok
  ) {
  | Fail(err) => Error(err)
  };
};
