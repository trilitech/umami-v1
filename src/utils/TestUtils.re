type waiting('a) =
  | Stop
  | Continue('a);

let rec waitForReduce = (acc, predicate, onPassed) => {
  switch (predicate(acc)) {
  | Stop => onPassed()
  | Continue(acc) =>
    Js.Global.setTimeout(() => waitForReduce(acc, predicate, onPassed), 1)
    ->ignore
  };
};

let waitFor = (predicate: unit => bool) => {
  let predicate = () => predicate() ? Stop : Continue();
  Promise.make(resolve => waitForReduce((), predicate, resolve));
};

let waitForReduce = (init, predicate): UmamiBase.FutureBase.t(unit) =>
  Promise.make(resolve => waitForReduce(init, predicate, resolve));
