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