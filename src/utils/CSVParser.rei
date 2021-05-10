module Encodings: {
  type element(_);

  let string: element(string);
  let bool: element(bool);
  let number: element(ReBigNumber.t);
  let custom: (~conv: string => option('a)) => element('a);
  let opt: element('a) => element(option('a));

  type row_repr(_);
  type row(_);

  let cell: element('a) => row_repr('a);
  let tup2: (element('a), element('b)) => row_repr(('a, 'b));
  let tup3:
    (element('a), element('b), element('c)) => row_repr(('a, 'b, 'c));
  let tup4:
    (element('a), element('b), element('c), element('d)) =>
    row_repr(('a, 'b, 'c, 'd));
  let tup5:
    (element('a), element('b), element('c), element('d), element('e)) =>
    row_repr(('a, 'b, 'c, 'd, 'e));

  let merge_rows: (row_repr('a), row_repr('b)) => row_repr(('a, 'b));

  let mkRow: row_repr('a) => row('a);

  /* Rows whose optional values are only accepted at the end (checked
     at runtime) */
  let mkNullableRow: row_repr('a) => row('a);
};

type row = int;
type col = int;

type error =
  | CannotParseNumber(row, col)
  | CannotParseBool(row, col)
  | CannotParseCustomValue(row, col)
  | CannotParseRow(row)
  | CannotParseCSV;

let parseRow: (~row: int=?, string, Encodings.row('a)) => result('a, error);

let parseCSV: (string, Encodings.row('a)) => result(list('a), error);
