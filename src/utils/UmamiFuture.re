type Errors.t +=
  | Timeout;

let () =
  Errors.registerHandler(
    "Future",
    fun
    | Timeout => "Timeout error"->Some
    | _ => None,
  );

let timeoutAfter = (f, ms) => {
  let timeout = ref(false);
  Future.make(resolve => {
    f->Future.get(r => resolve(r));

    Js.Global.setTimeout(
      () => timeout.contents ? () : Error(Timeout)->resolve,
      ms,
    )
    ->ignore;
  });
};
