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

type Errors.t +=
  | NotFAContract(string)
  | RegisterNotAFungibleToken(PublicKeyHash.t, TokenRepr.kind)
  | RegisterNotANonFungibleToken(PublicKeyHash.t, TokenRepr.kind);

type filter = [ | `Any | `FT | `NFT(PublicKeyHash.t, bool)];

let metadataToToken:
  (string, TokenContract.t, ReTaquitoTypes.Tzip12.metadata) => TokenRepr.t;

let registeredTokens: filter => Let.result(TokensLibrary.WithBalance.t);

let hiddenTokens: unit => Let.result(RegisteredTokens.t);

let addFungibleToken: (ConfigContext.env, Token.t) => Promise.t(unit);

let addNonFungibleToken:
  (ConfigContext.env, Token.t, PublicKeyHash.t, ReBigNumber.t) =>
  Promise.t(unit);

let registerNFTs:
  (TokensLibrary.WithBalance.t, PublicKeyHash.t) => Let.result(unit);

let updateNFTsVisibility:
  (PublicKeyHash.Map.map(Map.Int.t(unit)), ~hidden: bool) =>
  Let.result(RegisteredTokens.t);

let removeToken: (TokenRepr.t, ~pruneCache: bool) => Let.result(unit);

module Fetch: {
  // Returns the tokens contracts accounts have interacted with
  let tokenContracts:
    (
      ConfigContext.env,
      ~accounts: list(PublicKeyHash.t),
      ~kinds: list(TokenContract.kind)=?,
      ~limit: int64=?,
      ~index: int64=?,
      unit
    ) =>
    Promise.t(PublicKeyHash.Map.map(TokenContract.t));

  // Returns all the token contracts available on the chain
  let tokenRegistry:
    (
      ConfigContext.env,
      ~kinds: list(TokenContract.kind),
      ~limit: int64,
      ~index: int64,
      unit
    ) =>
    Promise.t(PublicKeyHash.Map.map(TokenContract.t));

  // Fetch the tokens of a user: those already registered, those to register,
  // and the next index to fetch from.
  let accountsTokens:
    (
      ConfigContext.env,
      ~accounts: list(PublicKeyHash.t),
      ~index: int,
      ~numberByAccount: int
    ) =>
    Promise.t(
      (array(TokensLibrary.Token.t), array(TokensLibrary.Token.t), int),
    );

  type fetched('tokens) = [ | `Cached('tokens) | `Fetched('tokens, int)];

  type fetchedNFTs = fetched(TokensLibrary.WithBalance.t);

  let accountNFTs:
    (
      ConfigContext.env,
      ~account: PublicKeyHash.t,
      ~numberByAccount: int,
      ~onTokens: (~total: int, ~lastToken: int) => unit,
      ~onStop: unit => bool,
      ~allowHidden: bool,
      ~fromCache: bool
    ) =>
    Promise.t(fetchedNFTs);

  let accountsTokensNumber:
    (ConfigContext.env, ~accounts: list(PublicKeyHash.t)) => Promise.t(int);

  type fetchedTokens = fetched(TokensLibrary.WithRegistration.t);

  let cachedFungibleTokensWithRegistration:
    ConfigContext.env =>
    Promise.t([> | `Cached(TokensLibrary.WithRegistration.t)]);

  let accountsFungibleTokensWithRegistration:
    (
      ConfigContext.env,
      ~accounts: list(PublicKeyHash.t),
      ~numberByAccount: int,
      ~onTokens: (~total: int, ~lastToken: int) => unit,
      ~onStop: unit => bool,
      ~fromCache: bool
    ) =>
    Promise.t(fetchedTokens);
};
