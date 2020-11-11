module Infix = {
  let (>>=) = Future.(>>=);
};

include Infix;
