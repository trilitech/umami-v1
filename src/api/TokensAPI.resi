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
  | NotFAContract(string)
  | RegisterNotAFungibleToken(PublicKeyHash.t, TokenRepr.kind)
  | RegisterNotANonFungibleToken(PublicKeyHash.t, TokenRepr.kind)

type filter = [#Any | #FT | #NFT(PublicKeyHash.t, bool)]

let metadataToToken: (
  Network.chainId,
  TokenContract.t,
  ReTaquitoTypes.Tzip12.metadata,
) => TokenRepr.t

let registeredTokens: filter => Promise.result<TokensLibrary.WithBalance.t>

let hiddenTokens: unit => Promise.result<RegisteredTokens.t>

let addTokenToCache: (Network.t, Token.t) => Promise.t<unit>

let addFungibleToken: (Network.t, Token.t) => Promise.t<unit>

let addNonFungibleToken: (Network.t, Token.t, PublicKeyHash.t, ReBigNumber.t) => Promise.t<unit>

let registerNFTs: (TokensLibrary.WithBalance.t, PublicKeyHash.t) => Promise.result<unit>

let updateNFTsVisibility: (
  PublicKeyHash.Map.map<Map.Int.t<unit>>,
  ~hidden: bool,
) => Promise.result<RegisteredTokens.t>

let removeToken: (TokenRepr.t, ~pruneCache: bool) => Promise.result<unit>

let cachedTokensWithRegistration: (
  Network.t,
  [#Any | #FT | #NFT],
) => Promise.t<[> #Cached(TokensLibrary.WithRegistration.t)]>

module Fetch: {
  // Returns the tokens contracts accounts have interacted with
  let tokenContracts: (
    Network.t,
    ~accounts: list<PublicKeyHash.t>,
    ~kinds: list<TokenContract.kind>=?,
    ~limit: int64=?,
    ~index: int64=?,
    unit,
  ) => Promise.t<PublicKeyHash.Map.map<TokenContract.t>>

  // Returns all the token contracts available on the chain
  let tokenRegistry: (
    Network.t,
    ~kinds: list<TokenContract.kind>,
    ~limit: int64,
    ~index: int64,
    unit,
  ) => Promise.t<PublicKeyHash.Map.map<TokenContract.t>>

  // Fetch the tokens of a user: those already registered, those to register,
  // and the next index to fetch from.
  let accountsTokens: (
    Network.t,
    ~accounts: list<PublicKeyHash.t>,
    ~index: int,
    ~numberByAccount: int,
  ) => Promise.t<(array<TokensLibrary.Token.t>, array<TokensLibrary.Token.t>, int)>

  type fetched<'tokens> = [#Cached('tokens) | #Fetched('tokens, int)]

  type fetchedNFTs = fetched<TokensLibrary.WithBalance.t>

  let accountNFTs: (
    Network.t,
    ~account: PublicKeyHash.t,
    ~numberByAccount: int,
    ~onTokens: (~total: int, ~lastToken: int) => unit,
    ~onStop: unit => bool,
    ~allowHidden: bool,
    ~fromCache: bool,
  ) => Promise.t<fetchedNFTs>

  let accountsTokensNumber: (Network.t, ~accounts: list<PublicKeyHash.t>) => Promise.t<int>

  type fetchedTokens = fetched<TokensLibrary.WithRegistration.t>

  let accountsFungibleTokensWithRegistration: (
    Network.t,
    ~accounts: list<PublicKeyHash.t>,
    ~numberByAccount: int,
    ~onTokens: (~total: int, ~lastToken: int) => unit,
    ~onStop: unit => bool,
    ~fromCache: bool,
  ) => Promise.t<fetchedTokens>
}
