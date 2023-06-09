include Belt.List;

let reduceGroupBy =
    (l: list('a), ~group: 'a => 'g, ~map: (option('b), 'a) => 'b)
    : list(('g, 'b)) =>
  l->reduce(
    [],
    (acc, e) => {
      let group: 'g = e->group;
      let groupValue = acc->getAssoc(group, (==));
      acc->setAssoc(group, map(groupValue, e), (==));
    },
  );

module Infix = {
  let (@:) = (e, l) => [e, ...l];
  let (@?) = (e, l) =>
    switch (e) {
    | None => l
    | Some(e) => [e, ...l]
    };
};

let rec firsts = (l, n) => {
  switch (l) {
  | [] => l
  | [h, ...t] => n == 0 ? [] : t->firsts(n - 1)->add(h)
  };
};

let rec findMap = (l, f) => {
  switch (l) {
  | [] => None
  | [h, ...t] =>
    switch (f(h)) {
    | Some(v) => Some(v)
    | None => findMap(t, f)
    }
  };
};
