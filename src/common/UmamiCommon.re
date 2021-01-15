module Infix = {
  let (>>=) = Future.(>>=);
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

  module String = {
    let countLeading = (str, c) => {
      let rec loop = n => str->Js.String2.charAt(n) == c ? loop(n + 1) : n;
      loop(0);
    };
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

    let fromOption = (v: option('a), e) =>
      switch (v) {
      | None => Error(e)
      | Some(v) => Ok(v)
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
      | [h, ...t] => n == 0 ? [] : t->firsts(n - 1)->List.add(h)
      };
    };
  };
};
