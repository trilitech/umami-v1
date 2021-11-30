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

let toTokenRepr = (tokenContract: TokenContract.t, token) => {
  let chain =
    token.network->Network.networkChain->Option.map(Network.getChainId);
  switch (token.symbol, token.name, token.decimals, chain) {
  | (Some(symbol), Some(name), Some(decimals), Some(chain)) =>
    TokenRepr.{
      kind: TokenContract.toTokenKind(tokenContract.kind, token.token_id),
      address: token.contract,
      alias: name,
      symbol,
      decimals,
      chain,
      asset: {
        ...defaultAsset,
        description: token.description,
        artifactUri: token.artifact_uri,
        displayUri: token.display_uri,
        thumbnailUri: token.thumbnail_uri,
        isTransferable: token.is_transferable->Option.getWithDefault(true),
        isBooleanAmount:
          token.is_boolean_amount->Option.getWithDefault(false),
        shouldPreferSymbol:
          token.should_prefer_symbol->Option.getWithDefault(false),
      },
    }
    ->Some
  | _ => None
  };
};

let requestPageSize = 50;

module Decode = {
  open JsonEx.Decode;

  let tokenBalanceDecoder = json => {
    balance: json |> field("balance", string) |> ReBigNumber.fromString,
    contract: json |> field("contract", PublicKeyHash.decoder),
    token_id: json |> field("token_id", int),
    network: json |> field("network", string),
    name: json |> optionalOrNull("name", string),
    symbol: json |> optionalOrNull("symbol", string),
    decimals: json |> optionalOrNull("decimals", int), //default: 0
    description: json |> optionalOrNull("description", string),
    artifact_uri: json |> optionalOrNull("artifact_uri", string),
    display_uri: json |> optionalOrNull("display_uri", string),
    thumbnail_uri: json |> optionalOrNull("thumbnail_uri", string),
    external_uri: json |> optionalOrNull("external_uri", string),
    is_transferable: json |> optionalOrNull("is_transferable", bool), // default: true
    is_boolean_amount: json |> optionalOrNull("is_boolean_amount", bool), // default: false
    should_prefer_symbol:
      json |> optionalOrNull("should_prefer_symbol", bool) //default: false
  };

  let decoder = json => {
    balances: json |> field("balances", array(tokenBalanceDecoder)),
    total: json |> field("total", int),
  };
};

module Encode = {
  open JsonEx.Encode;

  let tokenBalanceEncoder = t => {
    object_([
      ("balance", t.balance |> ReBigNumber.toString |> string),
      ("contract", t.contract |> PublicKeyHash.encoder),
      ("token_id", t.token_id |> int),
      ("network", t.network |> string),
      ("name", t.name |> nullable(string)),
      ("symbol", t.symbol |> nullable(string)),
      ("decimals", t.decimals |> nullable(int)), //default: 0
      ("description", t.description |> nullable(string)),
      ("artifact_uri", t.artifact_uri |> nullable(string)),
      ("display_uri", t.display_uri |> nullable(string)),
      ("thumbnail_uri", t.thumbnail_uri |> nullable(string)),
      ("external_uri", t.external_uri |> nullable(string)),
      ("is_transferable", t.is_transferable |> nullable(bool)), // default: true
      ("is_boolean_amount", t.is_boolean_amount |> nullable(bool)), // default: false
      ("should_prefer_symbol", t.should_prefer_symbol |> nullable(bool)) //default: false
    ]);
  };
};