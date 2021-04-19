module Unit = {
  type t = ReBigNumber.t;
  open ReBigNumber;

  let toBigNumber = x => x;
  let fromBigNumber = x =>
    x->isNaN || !x->isInteger || x->isNegative ? None : x->Some;

  let toNatString = toFixed;
  let fromNatString = s => s->fromString->fromBigNumber;
  let forceFromString = s => {
    let v = s->fromString;
    v->isNaN ? None : v->isInteger ? v->integerValue->Some : None;
  };

  let isValid = v =>
    v->fromNatString->Option.mapWithDefault(false, isInteger);

  let zero = fromString("0");

  let add = plus;

  module Infix = {
    let (+) = plus;
  };
};

type address = string;
type currency = (Unit.t, address);

let addCurrency = ((v, t), (v', t')) =>
  t == t' ? Some((Unit.add(v, v'), t)) : None;

type t = {
  address,
  alias: string,
  symbol: string,
  chain: string,
};
