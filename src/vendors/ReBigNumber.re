type t;

[@bs.module] [@bs.new] external fromString: string => t = "bignumber.js";

[@bs.send] external toString: t => string = "toString";
[@bs.send] external toFixed: t => string = "toFixed";
[@bs.send] external plus: (t, t) => t = "plus";
[@bs.send] external isNaN: t => bool = "isNaN";
[@bs.send] external integerValue: t => t = "integerValue";
[@bs.send] external isInteger: t => bool = "isInteger";
[@bs.send] external isNegative: t => bool = "isNegative";

let fromInt64 = i => i->Int64.to_string->fromString;
let toInt64 = i => i->toFixed->Int64.of_string;
