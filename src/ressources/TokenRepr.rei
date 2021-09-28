module Unit: {
  type t = pri ReBigNumber.t;

  let isNat: t => bool;

  type illformed =
    | NaN
    | Float
    | Negative;

  let fromBigNumber: ReBigNumber.t => result(t, illformed);
  let toBigNumber: t => ReBigNumber.t;

  let toNatString: (~decimals: int=?, t) => string;
  let fromNatString: string => result(t, illformed);

  let toStringDecimals: (t, int) => string;
  let fromStringDecimals: (string, int) => result(t, illformed);

  let formatString: (string, int) => Belt.Result.t(string, illformed);

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
