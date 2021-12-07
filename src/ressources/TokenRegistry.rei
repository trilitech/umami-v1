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

module Registered: {
  /** Representation of registered tokens for a user */
  type nftInfo = {
    holder: PublicKeyHash.t,
    hidden: bool,
    balance: ReBigNumber.t,
  };

  type kind =
    | FT
    | NFT(nftInfo);

  type contract = {
    contract: TokenContract.t,
    chain: string,
    tokens: Map.Int.t(kind) // effectively registered tokens by the user
  };

  include
    LocalStorage.StorageType with type t = PublicKeyHash.Map.map(contract);

  let getWithFallback: unit => Let.result(t);

  let isRegistered: (t, PublicKeyHash.t, int) => bool;
  let registerToken: (t, Token.t, kind) => t;
  let removeToken: (t, PublicKeyHash.t, int) => t;
  let isHidden: (t, PublicKeyHash.t, int) => bool;

  let keepTokens: (t, (PublicKeyHash.t, int, kind) => bool) => t;

  let updateNFTsVisibility:
    (t, PublicKeyHash.Map.map(Map.Int.t(unit)), bool) => t;
  let updateNFT: (t, PublicKeyHash.t, int, nftInfo) => t;
};

/** The cache is a representation of the already fetched tokens from the chain */
module Cache: {
  // Cached tokens, either complete or with missing values
  type token =
    | Full(Token.t)
    | Partial(TokenContract.t, BCD.tokenBalance, bool);

  let tokenId: token => int;
  let tokenAddress: token => PublicKeyHash.t;
  let tokenKind: token => TokenContract.kind;
  let tokenChain: token => option(string);
  let tokenName: token => option(string);
  let isFull: token => bool;

  let isNFT: token => bool;

  // A generic cached contract with its tokens
  type contract('tokens) = {
    address: PublicKeyHash.t,
    name: option(string),
    tokens: Map.Int.t('tokens),
  };

  type map('token) = PublicKeyHash.Map.map(contract('token));

  let empty: map('token);

  let getToken: (map('token), PublicKeyHash.t, int) => option('token);
  let updateToken:
    (
      map('token),
      PublicKeyHash.t,
      int,
      ~updatedValue: option('token) => option('token)
    ) =>
    map('token);
  let valuesToArray: map('token) => array('token);

  let keepTokens:
    (map('token), (PublicKeyHash.t, int, 'token) => bool) => map('token);

  // Cache in localStorage
  include LocalStorage.StorageType with type t = map(token);

  let getWithFallback: unit => Let.result(t);

  // Specific for localStorage version
  let getFullToken: (t, PublicKeyHash.t, int) => option(Token.t);

  let addToken: (t, token) => t;
  let removeToken: (t, token) => t;

  // Fetched from BetterCallDev
  type tokenWithBalance = (token, ReBigNumber.t);

  type withBalance = map(tokenWithBalance);

  let merge: (withBalance, withBalance) => withBalance;
  let getFullTokenWithBalance:
    (withBalance, PublicKeyHash.t, int) => option((Token.t, ReBigNumber.t));
};

let mergeAccountNFTs:
  (Registered.t, Cache.withBalance, PublicKeyHash.t) => Registered.t;

module Legacy: {
  module V1_3: {
    let version: Version.t;
    let mk: unit => Let.result(unit);
  };
};
