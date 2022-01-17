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
open Let;

module Encodings: {
  type Errors.t +=
    | IllformedEncoding;

  type element(_);
  type or_('a, 'b) = [ | `Left('a) | `Right('b)];

  let string: element(string);
  let bool: element(bool);
  let number: element(ReBigNumber.t);
  let custom: (~conv: string => result('a)) => element('a);
  let opt: element('a) => element(option('a));

  type row_repr(_);
  type row(_);

  /* Only valid inside a Or */
  let null: row_repr(unit);

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
  let or_: (row_repr('a), row_repr('b)) => row_repr(or_('a, 'b));

  let merge_rows: (row_repr('a), row_repr('b)) => row_repr(('a, 'b));

  let wellformed: row_repr('a) => bool;

  let mkRow: row_repr('a) => result(row('a));
};

type row = int;
type col = int;

type Errors.t +=
  | CannotParseNumber(row, col)
  | CannotParseBool(row, col)
  | CannotParseCustomValue(Errors.t, row, col)
  | CannotParseRow(row)
  | CannotParseCSV;

let parseRow: (~row: int=?, string, Encodings.row('a)) => result('a);

let parseCSV: (string, Encodings.row('a)) => result(list('a));
