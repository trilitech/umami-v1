module Unit: {
  type t = pri ReBigNumber.t;

  let isNat: t => bool;

  let fromBigNumber: ReBigNumber.t => option(t);
  let toBigNumber: t => ReBigNumber.t;

  let toNatString: t => string;
  let fromNatString: string => option(t);

  let toStringDecimals: (t, int) => string;
  let fromStringDecimals: (string, int) => option(t);

  let formatString: (string, int) => option(string);

  let zero: t;
  let forceFromString: string => option(t);

  let add: (t, t) => t;

  module Infix: {let (+): (t, t) => t;};
};

type address = PublicKeyHash.t;

type kind =
  | FA1_2
  | FA2(int);

type t = {
  kind,
  address,
  alias: string,
  symbol: string,
  chain: string,
  decimals: int,
};
