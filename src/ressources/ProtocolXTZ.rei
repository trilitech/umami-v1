type t;

let fromInt: int => t;
let zero: t;

module Infix: {
  let (+): (t, t) => t;
  let (-): (t, t) => t;
  let ( * ): (t, t) => t;
};

let fromString: string => option(t);

let toString: t => string;
