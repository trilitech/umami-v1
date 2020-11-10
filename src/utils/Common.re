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
