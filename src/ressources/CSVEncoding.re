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

open CSVParser;

type error =
  | Parser(TezosClient.CSVParser.error(PublicKeyHash.parsingError))
  | UnknownToken(string)
  | NoRows
  | CannotParseTokenAmount(ReBigNumber.t, int, int)
  | CannotParseTezAmount(ReBigNumber.t, int, int);

type t = list(Transfer.elt);

let addr = Encodings.custom(~conv=PublicKeyHash.buildImplicit);
let token = Encodings.custom(~conv=PublicKeyHash.buildContract);

let rowEncoding =
  Encodings.(mkRow(tup4(addr, number, opt(token), opt(number))));

let handleTezRow = (index, destination, amount) =>
  amount
  ->ReBigNumber.toString
  ->Tez.fromString
  ->ResultEx.fromOption(CannotParseTezAmount(amount, index, 2))
  ->Result.map(amount =>
      Transfer.makeSingleTezTransferElt(~destination, ~amount, ())
    );

let handleTokenRow =
    (tokens, index, destination, amount, token: PublicKeyHash.t) =>
  tokens
  ->Map.String.get((token :> string))
  ->Option.mapWithDefault(Error(UnknownToken((token :> string))), token =>
      amount
      ->Token.Unit.fromBigNumber
      ->ResultEx.fromOption(CannotParseTokenAmount(amount, index, 2))
      ->Result.map(amount =>
          Transfer.makeSingleTokenTransferElt(
            ~destination,
            ~amount,
            ~token,
            (),
          )
        )
    );

let handleRow = (tokens, index, row) =>
  switch (row) {
  | (destination, amount, None, _) =>
    handleTezRow(index, destination, amount)
  | (destination, amount, Some(token), _) =>
    handleTokenRow(tokens, index, destination, amount, token)
  };

let handleCSV = (rows, tokens) =>
  rows->List.mapWithIndex(handleRow(tokens))->ResultEx.collect;

let parseCSV = (content, ~tokens) => {
  let rows =
    parseCSV(content, rowEncoding)->ResultEx.mapError(e => Parser(e));
  switch (rows) {
  | Ok([]) => Error(NoRows)
  | Ok(rows) => handleCSV(rows, tokens)
  | Error(e) => Error(e)
  };
};

let handleCSVError = e =>
  e->CSVParser.(
       fun
       | Parser(CannotParseNumber(row, col)) =>
         I18n.csv#cannot_parse_number(row + 1, col + 1)
       | Parser(CannotParseBool(row, col)) =>
         I18n.csv#cannot_parse_boolean(row + 1, col + 1)
       | Parser(CannotParseCustomValue(e, row, col)) =>
         I18n.csv#cannot_parse_custom_value(
           PublicKeyHash.handleParsingError(e),
           row + 1,
           col + 1,
         )
       | Parser(CannotParseRow(row)) => I18n.csv#cannot_parse_row(row + 1)
       | Parser(CannotParseCSV) => I18n.csv#cannot_parse_csv
       | NoRows => I18n.csv#no_rows
       | CannotParseTokenAmount(v, row, col) =>
         I18n.csv#cannot_parse_token_amount(v, row + 1, col + 1)
       | CannotParseTezAmount(v, row, col) =>
         I18n.csv#cannot_parse_tez_amount(v, row + 1, col + 1)
       | UnknownToken(s) => I18n.csv#unknown_token(s)
     );
