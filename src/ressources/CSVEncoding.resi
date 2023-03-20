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

type Errors.t +=
  | UnknownToken(PublicKeyHash.t, option<int>)
  | NoRows
  | CannotParseTokenAmount(ReBigNumber.t, int, int)
  | CannotParseTezAmount(ReBigNumber.t, int, int)
  | FA1_2InvalidTokenId(PublicKeyHash.t)
  | FA2InvalidTokenId(PublicKeyHash.t)

type t = list<Protocol.Transfer.t>
/* Public key hash encoding */
let addr: CSVParser.Encodings.element<PublicKeyHash.t>
/* Contract hash encoding */
let contract: CSVParser.Encodings.element<PublicKeyHash.t>
/* Micheline encoding */
let michelson: CSVParser.Encodings.element<ReTaquitoTypes.MichelsonV1Expression.t>

type token = (PublicKeyHash.t, option<Umami.ReBigNumber.t>)

type transfer = ((PublicKeyHash.t, ReBigNumber.t), CSVParser.Encodings.or_<token, unit>)

type contractCall = (PublicKeyHash.t, string, ReTaquitoTypes.MichelsonV1Expression.t, option<ReBigNumber.t>)

/* CSV row encoding */
let rowEncoding: Promise.result<
  CSVParser.Encodings.row<CSVParser.Encodings.or_<transfer, contractCall>>,
>

let parseCSV: (string, ~tokens: TokensLibrary.WithRegistration.t) => result<t, Errors.t>
