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

/** Tokens metadata as defined by api.tzkt.io. See
   [https://api.tzkt.io/#operation/Tokens_GetTokenBalances] for the full format
*/

type metadata = {
  name: option(string),
  symbol: option(string),
  decimals: option(int), //default: 0
  description: option(string),
  artifactUri: option(string),
  displayUri: option(string),
  thumbnailUri: option(string),
  externalUri: option(string),
  isTransferable: option(bool), // default: true
  isBooleanAmount: option(bool), // default: false
  shouldPreferSymbol: option(bool), //default: false
  formats: option(array(Metadata.format)),
  creators: option(array(string)),
  tags: option(array(string)),
};

type contract = {
  alias: option(string),
  address: PublicKeyHash.t,
};

type tokenInfo = {
  contract,
  tokenId: ReBigNumber.t,
  standard: TokenContract.kind,
  metadata: option(metadata),
};

type t = {
  balance: ReBigNumber.t,
  tokenInfo,
};

type tokens = array(t);

let address: t => PublicKeyHash.t;
let tokenId: t => int;

let toTokenRepr: (option(Network.chainId), t) => option(TokenRepr.t);
let fromTokenRepr: TokenRepr.t => t;

let updateFromBuiltinTemplate: t => t;

let isNFT: t => bool;

/* Maximum tokens by request, which is 50 in the current API */
let requestPageSize: int;

module Decode: {
  let decoder: Json.Decode.decoder(t);
  let arrayDecoder: Json.Decode.decoder(array(t));
};

module Encode: {let encoder: Json.Encode.encoder(t);};

module Migration: {
  type bcd = {
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
    should_prefer_symbol: option(bool), //default: false
    formats: option(array(Metadata.format)),
    creators: option(array(string)),
    tags: option(array(string)),
  };

  let bcdDecoder: Json.Decode.decoder(bcd);

  let migrate: (TokenContract.t, bcd) => t;
};
