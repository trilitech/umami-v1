/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

module Encodings = {
  exception IllformedEncoder;

  type element(_) =
    | String: element(string)
    | Number: element(ReBigNumber.t)
    | Bool: element(bool)
    | Option(element('a)): element(option('a))
    | Custom(string => option('a)): element('a);

  let string = String;
  let number = Number;
  let bool = Bool;
  let custom = (~conv) => Custom(conv);
  let opt = elt => Option(elt);

  /* Directly encode rows as tuples from 1 to 5, instead of an
     heterogenous list. It simplifies the usage. */
  type row_repr(_) =
    | Cell(element('a)): row_repr('a)
    | Tup2(row_repr('a), row_repr('b)): row_repr(('a, 'b))
    | Tup3(row_repr('a), row_repr('b), row_repr('c))
      : row_repr(('a, 'b, 'c))
    | Tup4(row_repr('a), row_repr('b), row_repr('c), row_repr('d))
      : row_repr(('a, 'b, 'c, 'd))
    | Tup5(
        row_repr('a),
        row_repr('b),
        row_repr('c),
        row_repr('d),
        row_repr('e),
      )
      : row_repr(('a, 'b, 'c, 'd, 'e));

  /* This proxy can be used to enforce invariants in the future */
  type row('a) = row_repr('a);

  let rec isEndingNullable: type t. row_repr(t) => bool =
    fun
    | Cell(Option(_)) => true
    | Cell(_) => false
    | Tup2(r1, r2) => isEndingNullable(r1) ? isEndingNullable(r2) : false
    | Tup3(r1, r2, r3) => isEndingNullable(Tup2(r1, Tup2(r2, r3)))
    | Tup4(r1, r2, r3, r4) =>
      isEndingNullable(Tup2(r1, Tup2(r2, Tup2(r3, r4))))
    | Tup5(r1, r2, r3, r4, r5) =>
      isEndingNullable(Tup2(r1, Tup2(r2, Tup2(r3, Tup2(r4, r5)))));

  /* Combinators to generates row of up to 5 elements */
  let cell = elt => Cell(elt);
  let tup2 = (elt1, elt2) => Tup2(cell(elt1), cell(elt2));
  let tup3 = (elt1, elt2, elt3) =>
    Tup3(cell(elt1), cell(elt2), cell(elt3));
  let tup4 = (elt1, elt2, elt3, elt4) =>
    Tup4(cell(elt1), cell(elt2), cell(elt3), cell(elt4));
  let tup5 = (elt1, elt2, elt3, elt4, elt5) =>
    Tup5(cell(elt1), cell(elt2), cell(elt3), cell(elt4), cell(elt5));

  /* Combines to rows */
  let merge_rows = (r1, r2) => Tup2(r1, r2);

  /* Rows that can only support nullable values at the end of the row */
  let mkNullableRow = r => isEndingNullable(r) ? r : raise(IllformedEncoder);

  let mkRow = r => r;

  let rec length: type elt. row_repr(elt) => int =
    fun
    | Cell(_) => 1
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
  let b = v->Js.String.toLowerCase;
  b == "true"
    ? Ok(true)
    : b == "false" ? Ok(false) : Error(CannotParseBool(row, col));
};
let parseCustom = (v, conv, row, col) => {
  v->conv->ResultEx.fromOption(Error(CannotParseCustomValue(row, col)));
};

let rec parseElementRaw:
  type t. (string, Encodings.element(t), row, col) => result(t, error) =
  (value, enc, row, col) =>
    switch (enc) {
    | Encodings.Number => parseNumber(value, row, col)
    | Encodings.Bool => parseBool(value, row, col)
    | Encodings.Custom(conv) => parseCustom(value, conv, row, col)
    | Encodings.Option(enc) =>
      parseElementRaw(value, enc, row, col)->Result.map(v => Some(v))
    | Encodings.String => Ok(value)
    };

let parseElement = (v, enc, row, col) =>
  v
  ->Option.map(v => parseElementRaw(v, enc, row, col))
  ->Option.getWithDefault(Error(CannotParseRow(row)));

let parseNullableElement = (v, enc, row, col) =>
  v
  ->Option.map(v => v == "" ? Ok(None) : parseElementRaw(v, enc, row, col))
  ->Option.getWithDefault(Error(CannotParseRow(row)));

let rec parseRowRec:
  type t. (array(string), Encodings.row(t), row, col) => result(t, error) =
  (values, enc, row, col) =>
    Encodings.(
      switch (enc) {
      /* Parse a single cell */
      | Cell(Option(elt)) =>
        col >= values->Array.length
          ? Ok(None)
          : values[col]->parseNullableElement(Option(elt), row, col)

      | Cell(enc) =>
        col < values->Array.length
          ? values[col]->parseElement(enc, row, col)
          : Error(CannotParseRow(row))

      /* Parse a couple of two sub rows, and returns a couple */
      | Tup2(r1, r2) =>
        let v1 = parseRowRec(values, r1, row, col);
        let v2 = parseRowRec(values, r2, row, col + length(r1));
        ResultEx.map2(v1, v2, (v1, v2) => (v1, v2));

      /* These last three simply reuse Tup2 parsing and return a tuple of the correct type */
      | Tup3(r1, r2, r3) =>
        parseRowRec(values, Tup2(r1, Tup2(r2, r3)), row, col)
        ->Belt.Result.map(((a, (b, c))) => (a, b, c))

      | Tup4(r1, r2, r3, r4) =>
        parseRowRec(values, Tup2(r1, Tup3(r2, r3, r4)), row, col)
        ->Belt.Result.map(((a, (b, c, d))) => (a, b, c, d))

      | Tup5(r1, r2, r3, r4, r5) =>
        parseRowRec(values, Tup2(r1, Tup4(r2, r3, r4, r5)), row, col)
        ->Belt.Result.map(((a, (b, c, d, e))) => (a, b, c, d, e))
      }
    );

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
