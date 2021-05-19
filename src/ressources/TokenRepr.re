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

type t = {
  address,
  alias: string,
  symbol: string,
  chain: string,
};

type currency = (Unit.t, t);

let addCurrency = ((v, t), (v', t')) =>
  t == t' ? Some((Unit.add(v, v'), t)) : None;
