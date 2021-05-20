/* Different version from Js.List.filterMap */
let filterMap = (l, f) =>
  l->List.reduceReverse([], (acc, v) =>
    switch (f(v)) {
    | None => acc
    | Some(v) => [v, ...acc]
    }
  );
