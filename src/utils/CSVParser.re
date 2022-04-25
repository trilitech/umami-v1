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

open Promise;

module Encodings = {
  type Errors.t +=
    | IllformedEncoding;

  type or_('a, 'b) = [ | `Left('a) | `Right('b)];

  type element(_) =
    | String: element(string)
    | Number: element(ReBigNumber.t)
    | Bool: element(bool)
    | Option(element('a)): element(option('a))
    | Custom(string => result('a)): element('a);

  /* Directly encode rows as tuples from 1 to 5, instead of an
     heterogenous list. It simplifies the usage. */
  type row_repr(_) =
    | Null: row_repr(unit)
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
      : row_repr(('a, 'b, 'c, 'd, 'e))
    | Or(row_repr('a), row_repr('b)): row_repr(or_('a, 'b));

  let string = String;
  let number = Number;
  let bool = Bool;
  let custom = (~conv) => Custom(conv);
  let opt = elt => Option(elt);

  /* This proxy can be used to enforce invariants in the future */
  type row('a) = row_repr('a);

  let rec hasFixedLength: type t. row_repr(t) => option(int) =
    fun
    | Cell(_) => Some(1)
    | Null => Some(0)
    | Tup2(r1, r2) =>
      Option.map2(hasFixedLength(r1), hasFixedLength(r2), (l1, l2) =>
        l1 + l2
      )
    | Tup3(r1, r2, r3) => hasFixedLength(Tup2(r1, Tup2(r2, r3)))
    | Tup4(r1, r2, r3, r4) =>
      hasFixedLength(Tup2(r1, Tup2(r2, Tup2(r3, r4))))
    | Tup5(r1, r2, r3, r4, r5) =>
      hasFixedLength(Tup2(r1, Tup2(r2, Tup2(r3, Tup2(r4, r5)))))
    | Or(r, r') => {
        let l = hasFixedLength(r);
        let l' = hasFixedLength(r');
        l == l' ? l : None;
      };

  let sameLength = (r1, r2) =>
    Option.map2(hasFixedLength(r1), hasFixedLength(r2), (r1, r2) =>
      r1 == r2
    )
    ->Option.default(false);

  let rec noNull: type t. row_repr(t) => bool =
    fun
    | Null => false
    | Cell(_) => true
    | Tup2(r1, r2) => noNull(r1) && noNull(r2)
    | Tup3(r1, r2, r3) => noNull(Tup2(r1, Tup2(r2, r3)))
    | Tup4(r1, r2, r3, r4) => noNull(Tup2(r1, Tup2(r2, Tup2(r3, r4))))
    | Tup5(r1, r2, r3, r4, r5) =>
      noNull(Tup2(r1, Tup2(r2, Tup2(r3, Tup2(r4, r5)))))
    | Or(r1, r2) => noNull(r1) && noNull(r2);

  let rec wellformed: type t. row_repr(t) => bool =
    fun
    | Null => true
    | Cell(_) => true

    | Tup2(Or(r, r'), r2) =>
      wellformed(r)
      && wellformed(r')
      && sameLength(r, r')
      && wellformed(r2)

    | Tup2(r1, Or(r, r')) =>
      noNull(r1)
      && hasFixedLength(r1) != None
      && wellformed(r)
      && wellformed(r')

    | Tup2(r1, r2) => noNull(r1) && wellformed(r1) ? wellformed(r2) : false

    // Tup3-5 are normalized as series of tup2.
    | Tup3(r1, r2, r3) => wellformed(Tup2(r1, Tup2(r2, r3)))
    | Tup4(r1, r2, r3, r4) => wellformed(Tup2(r1, Tup2(r2, Tup2(r3, r4))))
    | Tup5(r1, r2, r3, r4, r5) =>
      wellformed(Tup2(r1, Tup2(r2, Tup2(r3, Tup2(r4, r5)))))

    // This last Or has no length checking since it is either checked by
    // tup2(or, _) or is at toplevel.
    | Or(r1, r2) => wellformed(r1) && wellformed(r2);

  /* Combinators to generates row of up to 5 elements */
  let null = Null;
  let cell = elt => Cell(elt);
  let tup2 = (elt1, elt2) => Tup2(cell(elt1), cell(elt2));
  let tup3 = (elt1, elt2, elt3) =>
    Tup3(cell(elt1), cell(elt2), cell(elt3));
  let tup4 = (elt1, elt2, elt3, elt4) =>
    Tup4(cell(elt1), cell(elt2), cell(elt3), cell(elt4));
  let tup5 = (elt1, elt2, elt3, elt4, elt5) =>
    Tup5(cell(elt1), cell(elt2), cell(elt3), cell(elt4), cell(elt5));
  let or_ = (r1, r2) => Or(r1, r2);

  /* Combines to rows */
  let merge_rows = (r1, r2) => Tup2(r1, r2);

  let mkRow = r => wellformed(r) ? Ok(r) : Error(IllformedEncoding);
};

type row = int;
type col = int;

type Errors.t +=
  | CannotParseNumber(row, col)
  | CannotParseBool(row, col)
  | CannotParseCustomValue(Errors.t, row, col)
  | CannotParseRow(row)
  | CannotParseCSV;

let () =
  Errors.registerHandler(
    "CSVParser",
    fun
    | CannotParseNumber(row, col) =>
      I18n.Csv.cannot_parse_number(row + 1, col + 1)->Some
    | CannotParseBool(row, col) =>
      I18n.Csv.cannot_parse_boolean(row + 1, col + 1)->Some
    | CannotParseCustomValue(e, row, col) =>
      I18n.Csv.cannot_parse_custom_value(e->Errors.toString, row + 1, col + 1)
      ->Some
    | CannotParseRow(row) => I18n.Csv.cannot_parse_row(row + 1)->Some
    | CannotParseCSV => I18n.Csv.cannot_parse_csv->Some
    | _ => None,
  );

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
  v->conv->Result.mapError(e => CannotParseCustomValue(e, row, col));
};

let rec parseElementRaw:
  type t. (string, Encodings.element(t), row, col) => result(t) =
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
  type t. (array(string), Encodings.row(t), row, col) => result((t, int)) =
  (values, enc, row, col) =>
    Encodings.(
      switch (enc) {
      | Null => Ok(((), 0))

      /* Parse a single cell */
      | Cell(Option(elt)) =>
        col >= values->Array.length
          ? Ok((None, 0))
          : values[col]
            ->parseNullableElement(Option(elt), row, col)
            ->Result.map(v => (v, 1))

      | Cell(enc) =>
        col < values->Array.length
          ? values[col]
            ->parseElement(enc, row, col)
            ->Result.map(v => (v, 1))
          : Error(CannotParseRow(row))

      /* Parse a couple of two sub rows, and returns a couple */
      | Tup2(r1, r2) =>
        let r1 = parseRowRec(values, r1, row, col);
        let r2 =
          r1->Result.flatMap(((_, l1)) =>
            parseRowRec(values, r2, row, col + l1)
          );
        Result.flatMap2(r1, r2, ((v1, l1), (v2, l2)) =>
          ((v1, v2), l1 + l2)->Ok
        );

      /* These last three simply reuse Tup2 parsing and return a tuple of the correct type */
      | Tup3(r1, r2, r3) =>
        parseRowRec(values, Tup2(r1, Tup2(r2, r3)), row, col)
        ->Result.map((((a, (b, c)), l)) => ((a, b, c), l))

      | Tup4(r1, r2, r3, r4) =>
        parseRowRec(values, Tup2(r1, Tup3(r2, r3, r4)), row, col)
        ->Result.map((((a, (b, c, d)), l)) => ((a, b, c, d), l))

      | Tup5(r1, r2, r3, r4, r5) =>
        parseRowRec(values, Tup2(r1, Tup4(r2, r3, r4, r5)), row, col)
        ->Result.map((((a, (b, c, d, e)), l)) => ((a, b, c, d, e), l))

      | Or(left, right) => parseOr(values, left, right, row, col)
      }
    )

and parseOr:
  type left right.
    (array(string), Encodings.row(left), Encodings.row(right), row, col) =>
    result((Encodings.or_(left, right), int)) =
  (values, enc1, enc2, row, col) =>
    switch (parseRowRec(values, enc1, row, col)) {
    | Ok((v, l)) => Ok((`Left(v), l))
    | Error(_) =>
      parseRowRec(values, enc2, row, col)
      ->Result.map(((v, l)) => (`Right(v), l))
    };

let removeComments = line => {
  let commentIndex = Js.String.indexOf("#", line);
  commentIndex != (-1)
    ? Js.String.substring(line, ~from=0, ~to_=commentIndex) : line;
};

let parseRow = (~row=0, value, encoding) => {
  Js.String.split(",", value)
  ->Array.map(Js.String.trim)
  ->parseRowRec(encoding, row, 0)
  ->Result.map(fst);
};

let parseRows = (rows, encoding) =>
  rows
  ->Js.Array2.reducei(
      (parsed, value, i) =>
        value == ""
          ? parsed : [parseRow(~row=i, value, encoding), ...parsed],
      [],
    )
  ->List.reverse;

let parseCSV = (value, encoding) => {
  Js.String.split("\n", value)
  ->Js.Array2.map(removeComments)
  ->parseRows(encoding)
  ->Result.collect;
};
