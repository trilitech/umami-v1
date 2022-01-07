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

module Token: {
  // Cached tokens, either complete or with missing values
  type t =
    | Full(Token.t)
    | Partial(TokenContract.t, BCD.tokenBalance, bool);

  let id: t => int;
  let address: t => PublicKeyHash.t;
  let kind: t => TokenContract.kind;
  let chain: t => option(string);
  let name: t => option(string);
  let symbol: t => option(string);
  let decimals: t => option(int);
  let isFull: t => bool;
  let isNFT: t => bool;

  let toTokenRepr:
    (~alias: string=?, ~symbol: string=?, ~decimals: int=?, t) =>
    option(TokenRepr.t);

  let uniqueKey: t => string;
};

module Contracts = PublicKeyHash.Map;
module Ids = Map.Int;

module Generic: {
  // A generic cached contract with its tokens
  type contract('tokens) = {
    address: PublicKeyHash.t,
    name: option(string),
    tokens: Map.Int.t('tokens),
  };

  type t('token) = Contracts.map(contract('token));

  let empty: t('token);

  let getToken: (t('token), PublicKeyHash.t, int) => option('token);
  let updateToken:
    (
      t('token),
      PublicKeyHash.t,
      int,
      ~updatedValue: option('token) => option('token)
    ) =>
    t('token);
  let reduce: (t('token), 'a, ('a, PublicKeyHash.t, int, 'token) => 'a) => 'a;

  let map: (t('token), 'token => 'mapped) => t('mapped);

  let valuesToArray: t('token) => array('token);

  let keepMap:
    (t('token), (PublicKeyHash.t, int, 'token) => option('mapped)) =>
    t('mapped);

  let keepTokens:
    (t('token), (PublicKeyHash.t, int, 'token) => bool) => t('token);

  let keepPartition:
    (t('token), (PublicKeyHash.t, int, 'token) => option(bool)) =>
    (t('token), t('token));

  let pickAny: t('token) => option((PublicKeyHash.t, int, 'token));
  let pickAnyAtAddress:
    (t('token), PublicKeyHash.t) => option((PublicKeyHash.t, int, 'token));
};

module WithBalance: {
  type token = (Token.t, ReBigNumber.t);
  type contract = Generic.contract(token);
  type t = Generic.t(token);

  let mergeAndUpdateBalance: (t, t) => t;
  let getFullToken:
    (t, PublicKeyHash.t, int) => option((TokenRepr.t, ReBigNumber.t));
};

module WithRegistration: {
  type token = (Token.t, bool);
  type contract = Generic.contract(token);
  type t = Generic.t(token);

  let keepAndSetRegistration:
    (Generic.t('token), RegisteredTokens.t, 'token => option(Token.t)) => t;

  let getFullToken: (t, PublicKeyHash.t, int) => option((TokenRepr.t, bool));
};

type t = Generic.t(Token.t);

let getFullToken: (t, PublicKeyHash.t, int) => option(TokenRepr.t);
let addToken: (t, Token.t) => t;
let removeToken: (t, Token.t) => t;

let invalidateCache: (t, [< | `Any | `FT | `NFT]) => t;
