type t;

let zero: t;

module Infix: {
  let (+): (t, t) => t;
  let (-): (t, t) => t;
  let ( * ): (t, t) => t;
};

let fromString: string => option(t);
let fromMutezString: string => t;
let fromMutezInt: int => t;
let unsafeToMutezInt: t => int;

let toString: t => string;
let toInt64: t => Int64.t;

let formatString: string => option(string);
