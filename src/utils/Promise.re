let ign = ignore;

include (
          FutureBase:
             (module type of FutureBase) with type t('a) := FutureBase.t('a)
        );

type result('a) = Result.t('a, Errors.t);
type t('a) = FutureBase.t(result('a));

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
  FutureBase.make(resolve => {
    f->get(r => resolve(r));

    Js.Global.setTimeout(
      () => timeout.contents ? () : Error(Timeout)->resolve,
      ms,
    )
    ->ign;
  });
};

type error = Errors.t;

let fromJs = FutureJs.fromPromise;

let rec _flatMapiSequentially = (values, transform, index) =>
  switch (values[0]) {
  | Some(a) =>
    value(Ok(a))
    ->flatMapOk(a =>
        transform(a, index)
        ->flatMapOk(_ =>
            values
            ->Array.sliceToEnd(1)
            ->_flatMapiSequentially(transform, index + 1)
          )
      )
  | None => value(Ok())
  };

let flatMapiSequentially = (values, transform) =>
  values->_flatMapiSequentially(transform, 0);
