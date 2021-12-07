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
    ~onTokens: (~total: int, ~lastToken: int) => unit=?,
    ~onStop: unit => bool=?,
    unit
  ) =>
  Promise.t((TokensLibrary.t, TokensLibrary.WithBalance.t, int));

let fetchAccountTokensStreamed:
  (
    ConfigContext.env,
    ~account: PublicKeyHash.t,
    ~index: int,
    ~numberByAccount: int,
    ~onTokens: (~total: int, ~lastToken: int) => unit,
    ~onStop: unit => bool
  ) =>
  Promise.t((TokensLibrary.WithBalance.t, int));

let fetchAccountsTokensRegistry:
  (
    ConfigContext.env,
    ~accounts: list(PublicKeyHash.t),
    ~index: int,
    ~numberByAccount: int
  ) =>
  Promise.t(
    (array(TokensLibrary.Token.t), array(TokensLibrary.Token.t), int),
  );

type fetched = [
  | `Cached(TokensLibrary.WithBalance.t)
  | `Fetched(TokensLibrary.WithBalance.t, int)
];

let fetchAccountNFTs:
  (
    ConfigContext.env,
    ~account: PublicKeyHash.t,
    ~numberByAccount: int,
    ~onTokens: (~total: int, ~lastToken: int) => unit,
    ~onStop: unit => bool,
    ~allowHidden: bool,
    ~fromCache: bool
  ) =>
  Promise.t(fetched);

let fetchAccountTokensNumber:
  (ConfigContext.env, ~account: PublicKeyHash.t) => Promise.t(int);
