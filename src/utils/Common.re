module Infix = {
  let (>>=) = Future.(>>=);
  // list elements composition
};

include Infix;

module Lib = {
  module Future = {
    let fromPromise = p =>
      Future.make(resolve => {
        p
        |> Js.Promise.then_(v => {
             resolve(Ok(v));
             Js.Promise.resolve();
           })
      });
  };

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
    let add = (l, e) => [e, ...l];

    let addOpt = (l, e) =>
      switch (e) {
      | None => l
      | Some(e) => [e, ...l]
      };

    let rec firsts = (l, n) => {
      switch (l) {
      | [] => l
      | [h, ...t] => n == 0 ? [] : t->firsts(n - 1)->Belt.List.add(h)
      };
    };
  };
};
