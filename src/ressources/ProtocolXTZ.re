module Infix = {
  open Int64;

  let (+) = add;
  let (-) = sub;
  let ( * ) = mul;
  let (/) = div;
  let (mod) = rem;
};

open Infix;

type t = Int64.t; // represented as mutez internally

let zero = Int64.zero;

let oneXtz = Int64.of_int(1000000);
let xtzExp10 = 6;

let toMutez = i => i * oneXtz;

let rec mulExp10 = (x, n) => {
  Int64.(n <= 0 ? x : mulExp10(x * of_int(10), Belt.Int.(n - 1)));
};

let int64OfString = s =>
  try(Some(s->Int64.of_string)) {
  | Failure(_) => None
  };

// `fromString` tries to read a string, returns None if
// it is not a formatted xtz
let fromString = (xtzStr): option(t) => {
  Int64.(
    switch (Js.String.split(".", xtzStr)) {
    | [|v|] => v->int64OfString->Belt.Option.map(toMutez)
    | [|integer, floating|] =>
      switch (int64OfString(integer), int64OfString(floating)) {
      | (Some(integer64), Some(floating64)) =>
        let floating64 = {
          let floatingMult =
            one->mulExp10(Belt.Int.(xtzExp10 - floating->Js.String.length));
          floating64 == zero ? zero : floating64 * floatingMult;
        };
        Some(integer64->toMutez + floating64);
      | (_, _) => None
      }
    | _ => None
    }
  );
};

let fromMutezInt = Int64.of_int;
let fromMutezString = Int64.of_string;

let toString = (xtz: t) => {
  open Int64;
  let integer = to_string(xtz / oneXtz);
  let floating = xtz mod oneXtz;
  let floatingStr = to_string(floating);
  let fLen = Js.String.length(floatingStr);

  let leading0 =
    fLen >= xtzExp10 && floating != zero
      ? "" : Js.String.repeat(Belt.Int.(xtzExp10 - fLen), "0");
  let floatingStr = leading0 ++ floatingStr;
  integer ++ "." ++ floatingStr;
};

let formatString = s => s->fromString->Belt.Option.map(toString);
