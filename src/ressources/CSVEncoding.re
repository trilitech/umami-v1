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
open CSVParser;

type Errors.t +=
  | UnknownToken(PublicKeyHash.t, option(int))
  | NoRows
  | CannotParseTokenAmount(ReBigNumber.t, int, int)
  | CannotParseTezAmount(ReBigNumber.t, int, int)
  | FA1_2InvalidTokenId(PublicKeyHash.t)
  | FA2InvalidTokenId(PublicKeyHash.t)
  | ContractCallsNotHandled;

let () =
  Errors.registerHandler(
    "CSVEncoding",
    fun
    | NoRows => I18n.Csv.no_rows->Some
    | CannotParseTokenAmount(v, row, col) =>
      I18n.Csv.cannot_parse_token_amount(v, row + 1, col + 1)->Some
    | CannotParseTezAmount(v, row, col) =>
      I18n.Csv.cannot_parse_tez_amount(v, row + 1, col + 1)->Some
    | UnknownToken(s, id) => I18n.Csv.unknown_token((s :> string), id)->Some
    | FA1_2InvalidTokenId(pkh) =>
      I18n.Csv.fa1_2_invalid_token_id((pkh :> string))->Some
    | FA2InvalidTokenId(pkh) =>
      I18n.Csv.fa2_invalid_token_id((pkh :> string))->Some
    | ContractCallsNotHandled => I18n.Csv.contract_calls_not_handled->Some
    | _ => None,
  );

type t = list(Transfer.t);

let addr = Encodings.custom(~conv=PublicKeyHash.buildImplicit);
let contract = Encodings.custom(~conv=PublicKeyHash.buildContract);
let michelson =
  Encodings.custom(
    ~conv=ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline,
  );

type token = (PublicKeyHash.t, option(Umami.ReBigNumber.t));

type transfer = (
  (PublicKeyHash.t, ReBigNumber.t),
  CSVParser.Encodings.or_(token, unit),
);

type contractCall = (
  PublicKeyHash.t,
  string,
  ReTaquitoParser.t,
  option(ReBigNumber.t),
);

let token: Encodings.row_repr(token) =
  Encodings.(tup2(contract, opt(number)));

let transfer: Encodings.row_repr(transfer) =
  Encodings.(merge_rows(tup2(addr, number), or_(token, null)));

let contractCall: Encodings.row_repr(contractCall) =
  Encodings.(tup4(contract, string, michelson, opt(number)));

let rowEncoding = Encodings.(or_(transfer, contractCall)->mkRow);

let handleTezRow = (index, destination, amount) =>
  amount
  ->ReBigNumber.toString
  ->Tez.fromString
  ->Result.fromOption(CannotParseTezAmount(amount, index, 2))
  ->Result.map(amount =>
      Transfer.makeSingleTezTransferElt(~destination, ~amount, ())
    );

let checkTokenId = (tokenId, (token: TokenRepr.t, registered)) =>
  switch (token.kind, tokenId) {
  | (FA1_2, Some(_)) => Error(FA1_2InvalidTokenId(token.address))
  | (FA2(_), None) => Error(FA2InvalidTokenId(token.address))
  | _ when !registered =>
    Error(
      UnknownToken(token.address, tokenId->Option.map(ReBigNumber.toInt)),
    )
  | _ => Ok(token)
  };

let handleTokenRow =
    (tokens, index, destination, amount, token: PublicKeyHash.t, tokenId) => {
  let%Res token =
    tokens
    ->TokensLibrary.WithRegistration.getFullToken(
        token,
        tokenId->Option.map(ReBigNumber.toInt)->Option.getWithDefault(0),
      )
    ->Option.mapWithDefault(
        Error(UnknownToken(token, tokenId->Option.map(ReBigNumber.toInt))),
        checkTokenId(tokenId),
      );

  amount
  ->Token.Unit.fromBigNumber
  ->Result.mapError(_ => CannotParseTokenAmount(amount, index, 2))
  ->Result.map(amount =>
      Transfer.makeSingleTokenTransferElt(~destination, ~amount, ~token, ())
    );
};

let handleRow = (tokens, index, row) =>
  switch (row) {
  | `Left((destination, amount), `Right ()) =>
    handleTezRow(index, destination, amount)
  | `Left((destination, amount), `Left(token, tokenId)) =>
    handleTokenRow(tokens, index, destination, amount, token, tokenId)
  | `Right(_) => Error(ContractCallsNotHandled)
  };

let handleCSV = (rows, tokens) =>
  rows->List.mapWithIndex(handleRow(tokens))->Result.collect;

let parseCSV = (content, ~tokens) => {
  let rows =
    rowEncoding->Result.flatMap(encoding => parseCSV(content, encoding));
  switch (rows) {
  | Ok([]) => Error(NoRows)
  | Ok(rows) => handleCSV(rows, tokens)
  | Error(e) => Error(e)
  };
};
