open Common;

module Infix = {
  open Int64;

  let (+) = add;
  let (-) = sub;
  let ( * ) = mul;
  let (/) = div;
  let (mod) = rem;
};

open Infix;

type t = Int64.t;
type xtz = t;

let fromInt = Int64.of_int;
let zero = Int64.zero;

let mutez = Int64.of_int(1000000);
let mutez0 = 6;

let rec mul10 = (x, n) => {
  Int64.(n <= 0 ? x : mul10(x * of_int(10), Belt.Int.(n - 1)));
};

let fromString = (xtzStr): option(t) => {
  Int64.(
    switch (Js.String.split(".", xtzStr)) {
    | [|v|] => Some(Int64.of_string(v) * mutez)
    | [|integer, floating|] =>
      let floating = {
        let floatingInt = of_string(floating);
        let floatingLength = to_string(floatingInt)->Js.String.length;
        let floatingMult =
          one->mul10(
            Belt.Int.(
              mutez0
              - (floating->Lib.String.countLeading("0") + floatingLength)
            ),
          );
        floatingInt == zero ? zero : floatingInt * floatingMult;
      };
      Some(mutez * of_string(integer) + floating);
    | _ => None
    }
  );
};

let toString = (xtz: t) => {
  open Int64;
  let integer = to_string(xtz / mutez);
  let floating = xtz mod mutez;
  let floatingStr = to_string(floating);
  let fLen = Js.String.length(floatingStr);

  let leading0 =
    fLen >= mutez0 && floating != zero
      ? "" : Js.String.repeat(Belt.Int.(mutez0 - fLen), "0");
  let floatingStr = leading0 ++ floatingStr;
  integer ++ "." ++ floatingStr;
};
