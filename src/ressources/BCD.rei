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

type tokenBalance = {
  balance: ReBigNumber.t,
  contract: PublicKeyHash.t,
  token_id: int,
  network: string,
  name: option(string),
  symbol: option(string),
  decimals: option(int), //default: 0
  description: option(string),
  artifact_uri: option(string),
  display_uri: option(string),
  thumbnail_uri: option(string),
  external_uri: option(string),
  is_transferable: option(bool), // default: true
  is_boolean_amount: option(bool), // default: false
  should_prefer_symbol: option(bool) //default: false
};

type t = {
  balances: array(tokenBalance),
  total: int,
};

let toTokenRepr: (TokenContract.t, tokenBalance) => option(TokenRepr.t);

module Decode: {
  let tokenBalanceDecoder: Json.Decode.decoder(tokenBalance);
  let decoder: Json.Decode.decoder(t);
};

module Encode: {let tokenBalanceEncoder: Json.Encode.encoder(tokenBalance);};
