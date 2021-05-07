module Encodings = {
  type element(_) =
    | String: element(string)
    | Number: element(ReBigNumber.t)
    | Bool: element(bool)
    | Custom(string => option('a)): element('a);

  let string = String;
  let number = Number;
  let bool = Bool;
  let custom = (~conv) => Custom(conv);

  /* Directly encode rows a tuples from 1 to 5, instead of an
     heterogenous list. It simplifies the usage. */
  type row(_) =
    | Sgl(element('a)): row('a)
    | Tup2(row('a), row('b)): row(('a, 'b))
    | Tup3(row('a), row('b), row('c)): row(('a, 'b, 'c))
    | Tup4(row('a), row('b), row('c), row('d)): row(('a, 'b, 'c, 'd))
    | Tup5(row('a), row('b), row('c), row('d), row('e))
      : row(('a, 'b, 'c, 'd, 'e));

  /* Combinators to generates row of up to 5 elements */
  let sgl = elt => Sgl(elt);
  let tup2 = (elt1, elt2) => Tup2(sgl(elt1), sgl(elt2));
  let tup3 = (elt1, elt2, elt3) => Tup3(sgl(elt1), sgl(elt2), sgl(elt3));
  let tup4 = (elt1, elt2, elt3, elt4) =>
    Tup4(sgl(elt1), sgl(elt2), sgl(elt3), sgl(elt4));
  let tup5 = (elt1, elt2, elt3, elt4, elt5) =>
    Tup5(sgl(elt1), sgl(elt2), sgl(elt3), sgl(elt4), sgl(elt5));

  /* Combines to rows */
  let merge_rows = (r1, r2) => Tup2(r1, r2);

  let rec length: type elt. row(elt) => int =
    fun
    | Sgl(_) => 1
    | Tup2(r1, r2) => length(r1) + length(r2)
    | Tup3(r1, r2, r3) => length(r1) + length(r2) + length(r3)
    | Tup4(r1, r2, r3, r4) =>
      length(r1) + length(r2) + length(r3) + length(r4)
    | Tup5(r1, r2, r3, r4, r5) =>
      length(r1) + length(r2) + length(r3) + length(r4) + length(r5);
};

type row = int;
type col = int;

type error =
  | CannotParseNumber(row, col)
  | CannotParseBool(row, col)
  | CannotParseCustomValue(row, col)
  | CannotParseRow(row)
  | CannotParseCSV;

let parseNumber = (v, row, col) => {
  let n = ReBigNumber.fromString(v);
  n->ReBigNumber.isNaN ? Error(CannotParseNumber(row, col)) : Ok(n);
};

/* Parse every case of "true" and "false" */
let parseBool = (v, row, col) => {
  let b = v->String.lowercase_ascii;
  b == "true"
    ? Ok(true)
    : b == "false" ? Ok(false) : Error(CannotParseBool(row, col));
};

let parseCustom = (v, conv, row, col) => {
  v->conv->ResultEx.fromOption(Error(CannotParseCustomValue(row, col)));
};

let parseElement:
  type t. (string, Encodings.element(t), row, col) => result(t, error) =
  (value, enc, row, col) =>
    switch (enc) {
    | Encodings.Number => parseNumber(value, row, col)
    | Encodings.Bool => parseBool(value, row, col)
    | Encodings.Custom(conv) => parseCustom(value, conv, row, col)
    | Encodings.String => Ok(value)
    };

let parseOptionElement = (v, enc, row, col) =>
  v
  ->Option.map(v => parseElement(v, enc, row, col))
  ->Option.getWithDefault(Error(CannotParseRow(row)));

let rec parseRowRec:
  type t. (array(string), Encodings.row(t), row, col) => result(t, error) =
  (values, enc, row, col) =>
    switch (enc) {
    /* Parse a single row */
    | Encodings.Sgl(enc) =>
      col < values->Array.length
        ? values[col]->parseOptionElement(enc, row, col)
        : Error(CannotParseRow(row))

    /* Parse a couple of two sub rows, and returns a couple */
    | Encodings.Tup2(r1, r2) =>
      let v1 = parseRowRec(values, r1, row, col);
      let v2 = parseRowRec(values, r2, row, col + Encodings.length(r1));
      ResultEx.map2(v1, v2, (v1, v2) => (v1, v2));

    /* These last three simply reuse Tup2 parsing and return a tuple of the correct type */
    | Encodings.Tup3(r1, r2, r3) =>
      parseRowRec(values, Tup2(r1, Tup2(r2, r3)), row, col)
      ->Belt.Result.map(((a, (b, c))) => (a, b, c))

    | Encodings.Tup4(r1, r2, r3, r4) =>
      parseRowRec(values, Tup2(r1, Tup3(r2, r3, r4)), row, col)
      ->Belt.Result.map(((a, (b, c, d))) => (a, b, c, d))

    | Encodings.Tup5(r1, r2, r3, r4, r5) =>
      parseRowRec(values, Tup2(r1, Tup4(r2, r3, r4, r5)), row, col)
      ->Belt.Result.map(((a, (b, c, d, e))) => (a, b, c, d, e))
    };

let parseRow = (~row=0, value, encoding) => {
  Js.String.split(",", value)
  ->Array.map(Js.String.trim)
  ->parseRowRec(encoding, row, 0);
};

let parseCSV = (value, encoding) => {
  Js.String.split("\n", value)
  ->Js.Array2.filter(l => l != "")
  ->Js.Array2.mapi((l, i) => parseRow(~row=i, l, encoding))
  ->List.fromArray
  ->ResultEx.collect;
};
