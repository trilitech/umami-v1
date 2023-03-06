/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

open CSVParser

type Errors.t +=
  | UnknownToken(PublicKeyHash.t, option<int>)
  | NoRows
  | CannotParseTokenAmount(ReBigNumber.t, int, int)
  | CannotParseTezAmount(ReBigNumber.t, int, int)
  | FA1_2InvalidTokenId(PublicKeyHash.t)
  | FA2InvalidTokenId(PublicKeyHash.t)

let () = Errors.registerHandler("CSVEncoding", x =>
  switch x {
  | NoRows => I18n.Csv.no_rows->Some
  | CannotParseTokenAmount(v, row, col) =>
    I18n.Csv.cannot_parse_token_amount(v, row + 1, col + 1)->Some
  | CannotParseTezAmount(v, row, col) => I18n.Csv.cannot_parse_tez_amount(v, row + 1, col + 1)->Some
  | UnknownToken(s, id) => I18n.Csv.unknown_token((s :> string), id)->Some
  | FA1_2InvalidTokenId(pkh) => I18n.Csv.fa1_2_invalid_token_id((pkh :> string))->Some
  | FA2InvalidTokenId(pkh) => I18n.Csv.fa2_invalid_token_id((pkh :> string))->Some
  | _ => None
  }
)

type t = list<Protocol.Transfer.t>

let addr = Encodings.custom(~conv=PublicKeyHash.build)
let contract = Encodings.custom(~conv=PublicKeyHash.buildContract)
let michelson = Encodings.custom(
  ~conv=(x => x->ProtocolOptions.TransactionParameters.MichelineMichelsonV1Expression.parseMicheline->Result.map(Option.getExn)),
)

type token = (PublicKeyHash.t, option<Umami.ReBigNumber.t>)

type transfer = ((PublicKeyHash.t, ReBigNumber.t), CSVParser.Encodings.or_<token, unit>)

type contractCall = (PublicKeyHash.t, string, ReTaquitoTypes.MichelsonV1Expression.t, option<ReBigNumber.t>)

let token: Encodings.row_repr<token> = {
  open Encodings
  tup2(contract, opt(number))
}

let transfer: Encodings.row_repr<transfer> = {
  open Encodings
  merge_rows(tup2(addr, number), or_(token, null))
}

let contractCall: Encodings.row_repr<contractCall> = {
  open Encodings
  tup4(contract, string, michelson, opt(number))
}

let rowEncoding = {
  open Encodings
  or_(transfer, contractCall)->mkRow
}

let handleTezRow = (~parameter=?, ~entrypoint=?, index, destination, amount) =>
  amount
  ->ReBigNumber.toString
  ->Tez.fromString
  ->Result.fromOption(CannotParseTezAmount(amount, index, 2))
  ->Result.map(amount =>
    ProtocolHelper.Transfer.makeSimpleTez(~parameter?, ~entrypoint?, ~destination, ~amount, ())
  )

let checkTokenId = (tokenId, (token: TokenRepr.t, registered)) =>
  switch (token.kind, tokenId) {
  | (FA1_2, Some(_)) => Error(FA1_2InvalidTokenId(token.address))
  | (FA2(_), None) => Error(FA2InvalidTokenId(token.address))
  | _ if !registered => Error(UnknownToken(token.address, tokenId->Option.map(ReBigNumber.toInt)))
  | _ => Ok(token)
  }

let handleTokenRow = (source, tokens, index, destination, amount, token: PublicKeyHash.t, tokenId) =>
  tokens
  ->TokensLibrary.WithRegistration.getFullToken(
    token,
    tokenId->Option.map(ReBigNumber.toInt)->Option.getWithDefault(0),
  )
  ->Option.mapWithDefault(
    Error(UnknownToken(token, tokenId->Option.map(ReBigNumber.toInt))),
    checkTokenId(tokenId),
  )
  ->Result.flatMap(token =>
    amount
    ->Token.Unit.fromFloatBigNumber(token.decimals)
    ->Result.mapError(_ => CannotParseTokenAmount(amount, index, 2))
    ->Result.map(amount =>
      ProtocolHelper.Transfer.makeSimpleToken(~source, ~destination, ~amount, ~token, ())
    )
  )

let handleRow = (source, tokens, index, row) =>
  switch row {
  | #Left((destination, amount), #Right()) => handleTezRow(index, destination, amount)
  | #Left((destination, amount), #Left(token, tokenId)) =>
    handleTokenRow(source, tokens, index, destination, amount, token, tokenId)
  | #Right(kt, entrypoint, parameter, amount) =>
    handleTezRow(~parameter, ~entrypoint, index, kt, amount->Option.default(ReBigNumber.zero))
  }

let handleCSV = (rows, source, tokens) => rows->List.mapWithIndex(handleRow(source, tokens))->Result.collect

let parseCSV = (content, ~source, ~tokens) => {
  let rows = rowEncoding->Result.flatMap(encoding => parseCSV(content, encoding))
  switch rows {
  | Ok(list{}) => Error(NoRows)
  | Ok(rows) => handleCSV(rows, source, tokens)
  | Error(e) => Error(e)
  }
}
