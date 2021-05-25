module Encodings: {
  type element(_, 'error);

  let string: element(string, 'error);
  let bool: element(bool, 'error);
  let number: element(ReBigNumber.t, 'error);
  let custom: (~conv: string => result('a, 'error)) => element('a, 'error);
  let opt: element('a, 'error) => element(option('a), 'error);

  type row_repr(_, 'error);
  type row(_, 'error);

  let cell: element('a, 'error) => row_repr('a, 'error);
  let tup2:
    (element('a, 'error), element('b, 'error)) =>
    row_repr(('a, 'b), 'error);
  let tup3:
    (element('a, 'error), element('b, 'error), element('c, 'error)) =>
    row_repr(('a, 'b, 'c), 'error);
  let tup4:
    (
      element('a, 'error),
      element('b, 'error),
      element('c, 'error),
      element('d, 'error)
    ) =>
    row_repr(('a, 'b, 'c, 'd), 'error);
  let tup5:
    (
      element('a, 'error),
      element('b, 'error),
      element('c, 'error),
      element('d, 'error),
      element('e, 'error)
    ) =>
    row_repr(('a, 'b, 'c, 'd, 'e), 'error);

  let merge_rows:
    (row_repr('a, 'error), row_repr('b, 'error)) =>
    row_repr(('a, 'b), 'error);

  let mkRow: row_repr('a, 'error) => row('a, 'error);

  /* Rows whose optional values are only accepted at the end (checked
     at runtime) */
  let mkNullableRow: row_repr('a, 'error) => row('a, 'error);
};

type row = int;
type col = int;

type error('error) =
  | CannotParseNumber(row, col)
  | CannotParseBool(row, col)
  | CannotParseCustomValue('error, row, col)
  | CannotParseRow(row)
  | CannotParseCSV;

let parseRow:
  (~row: int=?, string, Encodings.row('a, 'error)) =>
  result('a, error('error));

let parseCSV:
  (string, Encodings.row('a, 'error)) => result(list('a), error('error));
