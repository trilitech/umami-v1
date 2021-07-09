module Unit: {
  type t = pri ReBigNumber.t;

  let fromBigNumber: ReBigNumber.t => option(t);
  let toBigNumber: t => ReBigNumber.t;

  let toNatString: t => string;
  let fromNatString: string => option(t);

  let isValid: string => bool;

  let zero: t;

  let forceFromString: string => option(t);

  let add: (t, t) => t;

  module Infix: {let (+): (t, t) => t;};
};

type address = PublicKeyHash.t;

type t = {
  address,
  alias: string,
  symbol: string,
  chain: string,
};
