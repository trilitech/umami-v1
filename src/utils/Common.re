module Infix = {
  let (>>=) = Future.(>>=);
  // list elements composition
  let (@:) = (e, l) => Belt.List.add(l, e);
  let (@?) = (e, l) =>
    switch (e) {
    | None => l
    | Some(e) => l->Belt.List.add(e)
    };
  let (@?$) = (e1, e2) => e1 @? e2 @@ [];
  let (@$?) = (e1, e2) => e1 @: e2 @? [];
  let (@??) = (e1, e2) => e1 @? e2 @? [];
  let (@$$) = (e1, e2) => e1 @: e2 @: [];
};

include Infix;

module Lib = {
  module Option = {
    let iter = (o, f) => {
      switch (o) {
      | Some(v) => f(v)
      | None => ()
      };
    };

    let onlyIf = (b, f) => b ? Some(f()) : None;

    let rec firstSome = l => {
      switch (l) {
      | [] => None
      | [Some(_) as h, ..._] => h
      | [None, ...t] => firstSome(t)
      };
    };
  };

  module Result = {
    let iterOk = (r, f) => {
      switch (r) {
      | Ok(v) => f(v)
      | Error(_) => ()
      };
    };
  };

  module List = {
    let rec firsts = (l, n) => {
      switch (l) {
      | [] => l
      | [h, ...t] => n == 0 ? [] : t->firsts(n - 1)->Belt.List.add(h)
      };
    };
  };
};
