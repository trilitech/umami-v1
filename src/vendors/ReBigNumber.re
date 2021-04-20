type t;

[@bs.module] [@bs.new] external fromString: string => t = "bignumber.js";

[@bs.send] external toString: t => string = "toString";
[@bs.send] external toFixed: t => string = "toFixed";
[@bs.send] external plus: (t, t) => t = "plus";
[@bs.send] external toFixedZero: (t, [@bs.as "0"] _) => string = "toFixed";
[@bs.send] external isNaN: t => bool = "isNaN";
