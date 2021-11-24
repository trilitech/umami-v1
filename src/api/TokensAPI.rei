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
  | NotFAContract(string);

type filter = [ | `Any | `FT | `NFT(PublicKeyHash.t, bool)];

let registeredTokens: filter => Let.result(TokenRegistry.Cache.t);

let addFungibleToken: (ConfigContext.env, Token.t) => Promise.t(unit);

let addNonFungibleToken:
  (ConfigContext.env, Token.t, PublicKeyHash.t) => Promise.t(unit);

let registerNFTs:
  (TokenRegistry.Cache.t, PublicKeyHash.t) => Let.result(unit);

let removeToken: (Token.t, ~pruneCache: bool) => Let.result(unit);

let fetchTokenContracts:
  (
    ConfigContext.env,
    ~accounts: list(PublicKeyHash.t),
    ~kinds: list(TokenContract.kind)=?,
    ~limit: int64=?,
    ~index: int64=?,
    unit
  ) =>
  Promise.t(PublicKeyHash.Map.map(TokenContract.t));

let fetchTokenRegistry:
  (
    ConfigContext.env,
    ~kinds: list(TokenContract.kind),
    ~limit: int64,
    ~index: int64,
    unit
  ) =>
  Promise.t(PublicKeyHash.Map.map(TokenContract.t));

let fetchAccountsTokens:
  (
    ConfigContext.env,
    ~accounts: list(PublicKeyHash.t),
    ~index: int,
    ~numberByAccount: int,
    ~withFullCache: bool
  ) =>
  Promise.t((TokenRegistry.Cache.t, int));

let fetchAccountTokensStreamed:
  (
    ConfigContext.env,
    ~account: PublicKeyHash.t,
    ~index: int,
    ~numberByAccount: int,
    ~onTokens: (~fetchedTokens: TokenRegistry.Cache.t, ~nextIndex: int) => unit,
    ~withFullCache: bool
  ) =>
  Promise.t((TokenRegistry.Cache.t, int));

let fetchAccountsTokensRegistry:
  (
    ConfigContext.env,
    ~accounts: list(PublicKeyHash.t),
    ~index: int,
    ~numberByAccount: int
  ) =>
  Promise.t(
    (
      array(TokenRegistry.Cache.token),
      array(TokenRegistry.Cache.token),
      int,
    ),
  );

type fetched = [
  | `Cached(TokenRegistry.Cache.t)
  | `Fetched(TokenRegistry.Cache.t, int)
];

let fetchAccountNFTs:
  (
    ConfigContext.env,
    ~account: PublicKeyHash.t,
    ~numberByAccount: int,
    ~onTokens: (~fetchedTokens: TokenRegistry.Cache.t, ~nextIndex: int) => unit,
    ~allowHidden: bool,
    ~fromCache: bool
  ) =>
  Promise.t(fetched);
