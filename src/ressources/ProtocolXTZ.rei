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

let toString: t => string;

let formatString: string => option(string);
