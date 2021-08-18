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

type error =
  | Parser(TezosClient.CSVParser.error(PublicKeyHash.parsingError))
  | UnknownToken(string)
  | NoRows
  | CannotParseTokenAmount(ReBigNumber.t, int, int)
  | CannotParseTezAmount(ReBigNumber.t, int, int);

type t = list(Transfer.elt);
/* Public key hash encoding */
let addr:
  CSVParser.Encodings.element(PublicKeyHash.t, PublicKeyHash.parsingError);
/* Contract hash encoding */
let token:
  CSVParser.Encodings.element(PublicKeyHash.t, PublicKeyHash.parsingError);
/* CSV row encoding */
let rowEncoding:
  CSVParser.Encodings.row(
    (
      PublicKeyHash.t,
      ReBigNumber.t,
      option(PublicKeyHash.t),
      option(ReBigNumber.t),
    ),
    PublicKeyHash.parsingError,
  );

let parseCSV:
  (string, ~tokens: Map.String.t(TokenRepr.t)) => result(t, error);

let handleCSVError: error => string;