module Encodings: {
  type element(_);

  let string: element(string);
  let bool: element(bool);
  let number: element(ReBigNumber.t);
  let custom: (~conv: string => option('a)) => element('a);

  type row(_);

  let sgl: element('a) => row('a);
  let tup2: (element('a), element('b)) => row(('a, 'b));
  let tup3: (element('a), element('b), element('c)) => row(('a, 'b, 'c));
  let tup4:
    (element('a), element('b), element('c), element('d)) =>
    row(('a, 'b, 'c, 'd));
  let tup5:
    (element('a), element('b), element('c), element('d), element('e)) =>
    row(('a, 'b, 'c, 'd, 'e));

  let merge_rows: (row('a), row('b)) => row(('a, 'b));
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
