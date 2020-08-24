let getOk = (result, sink) =>
  switch (result) {
  | Ok(value) => sink(value)
  | Error(_) => ()
  };