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
  let isFull: t => bool;
  let isNFT: t => bool;
};

module Generic: {
  // A generic cached contract with its tokens
  type contract('tokens) = {
    address: PublicKeyHash.t,
    name: option(string),
    tokens: Map.Int.t('tokens),
  };

  type t('token) = PublicKeyHash.Map.map(contract('token));

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
  let valuesToArray: t('token) => array('token);

  let keepMap:
    (t('token), (PublicKeyHash.t, int, 'token) => option('mapped)) =>
    t('mapped);

  let keepTokens:
    (t('token), (PublicKeyHash.t, int, 'token) => bool) => t('token);
};

module WithBalance: {
  type token = (Token.t, ReBigNumber.t);
  type contract = Generic.contract(token);
  type t = Generic.t(token);

  let mergeAndUpdateBalance: (t, t) => t;
  let getFullToken:
    (t, PublicKeyHash.t, int) => option((TokenRepr.t, ReBigNumber.t));
};

type t = Generic.t(Token.t);

let getFullToken: (t, PublicKeyHash.t, int) => option(TokenRepr.t);
let addToken: (t, Token.t) => t;
let removeToken: (t, Token.t) => t;

let invalidateCache: (t, [< | `Any | `FT | `NFT]) => t;
