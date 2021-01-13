type t;
type xtz = t;

let fromInt: int => xtz;
let zero: xtz;

module Infix: {
  let (+): (xtz, xtz) => xtz;
  let (-): (xtz, xtz) => xtz;
  let ( * ): (xtz, xtz) => xtz;
};

let fromString: string => option(xtz);

let toString: xtz => string;
