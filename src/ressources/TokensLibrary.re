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

module Token = {
  type t =
    | Full(TokenRepr.t)
    | Partial(TokenContract.t, BCD.tokenBalance, bool);

  let id =
    fun
    | Full(t) => TokenRepr.id(t)
    | Partial(_, bcd, _) => bcd.BCD.token_id;

  let address =
    fun
    | Full(t) => t.TokenRepr.address
    | Partial(_, bcd, _) => bcd.BCD.contract;

  let name =
    fun
    | Full(t) => t.TokenRepr.alias->Some
    | Partial(_, bcd, _) => bcd.BCD.name;

  let kind =
    fun
    | Full(t) => TokenContract.fromTokenKind(t.TokenRepr.kind)
    | Partial(tc, _, _) => tc.TokenContract.kind;

  let chain =
    fun
    | Full(t) => Some(t.TokenRepr.chain)
    | Partial(_, bcd, _) =>
      bcd.BCD.network->Network.networkChain->Option.map(Network.getChainId);

  let isNFT =
    fun
    | Full(t) => t->TokenRepr.isNFT
    | Partial(_, _, _) => false;

  let isFull =
    fun
    | Full(_) => true
    | Partial(_, _, _) => false;
};

module Generic = {
  type contract('tokens) = {
    address: PublicKeyHash.t,
    name: option(string),
    tokens: Map.Int.t('tokens),
  };

  type t('token) = PublicKeyHash.Map.map(contract('token));

  let empty = PublicKeyHash.Map.empty;

  let getToken = (cache, pkh, tokenId) => {
    cache
    ->PublicKeyHash.Map.get(pkh)
    ->Option.flatMap(({tokens}) => tokens->Map.Int.get(tokenId));
  };

  let updateToken = (cache, pkh, tokenId, ~updatedValue) => {
    let f = contract => {
      let tokens =
        contract
        ->Option.mapWithDefault(Map.Int.empty, c => c.tokens)
        ->Map.Int.update(tokenId, updatedValue);
      contract
      ->Option.mapWithDefault({name: None, address: pkh, tokens}, contract =>
          {...contract, tokens}
        )
      ->Some;
    };
    cache->PublicKeyHash.Map.update(pkh, f);
  };

  let valuesToArray = cache => {
    cache
    ->PublicKeyHash.Map.valuesToArray
    ->Array.map(c => c.tokens->Map.Int.valuesToArray)
    ->Array.concatMany;
  };

  let keepTokens = (cache, f) =>
    cache->PublicKeyHash.Map.reduce(
      PublicKeyHash.Map.empty,
      (cache, pkh, c) => {
        let tokens =
          c.tokens->Map.Int.keep((id, token) => f(pkh, id, token));
        tokens->Map.Int.isEmpty
          ? cache : cache->PublicKeyHash.Map.set(pkh, {...c, tokens});
      },
    );
};

module WithBalance = {
  open Token;
  open Generic;

  type token = (Token.t, ReBigNumber.t);
  type contract = Generic.contract(token);
  type t = Generic.t(token);

  // map2 is always the newly added tokens
  let mergeAndUpdateBalance = (map1, map2) => {
    let pickToken = ((t1, b1), (t2, b2)) =>
      switch (t1, t2) {
      | (Full(_), Full(_)) => (t1, b2)
      | (Full(_) as t, Partial(_, _, _))
      | (Partial(_, _, _), Full(_) as t) => (t, b2)
      | _ => (t1, b1)
      };
    let mergeTokens = (_, t1, t2) =>
      switch (t1, t2) {
      | (None, None) => None
      | (Some(_) as t, None)
      | (None, Some(_) as t) => t
      | (Some(t1), Some(t2)) => pickToken(t1, t2)->Some
      };
    let mergeContracts = (key, c1, c2) =>
      switch (c1, c2) {
      | (None, None) => None
      | (Some(_) as c, None)
      | (None, Some(_) as c) => c
      | (Some(c1), Some(c2)) =>
        {
          address: key,
          name: Option.keep(c1.name, c2.name),
          tokens: Map.Int.merge(c1.tokens, c2.tokens, mergeTokens),
        }
        ->Some
      };
    PublicKeyHash.Map.merge(map1, map2, mergeContracts);
  };

  let getFullToken = (map, pkh, tokenId) => {
    switch (map->getToken(pkh, tokenId)) {
    | Some((Full(t), b)) => Some((t, b))
    | _ => None
    };
  };
};

type t = Generic.t(Token.t);

let getFullToken = (map, pkh, tokenId) => {
  switch (map->Generic.getToken(pkh, tokenId)) {
  | Some(Token.Full(t)) => Some(t)
  | _ => None
  };
};

let addToken = (map, token) =>
  Generic.updateToken(
    map, Token.address(token), Token.id(token), ~updatedValue=_ =>
    Some(token)
  );

let removeToken = (map, token) => {
  let f = contract => {
    let tokens =
      contract
      ->Option.mapWithDefault(Map.Int.empty, c => c.Generic.tokens)
      ->Map.Int.remove(Token.id(token));
    contract->Option.map(contract => {...contract, tokens});
  };
  map->PublicKeyHash.Map.update(Token.address(token), f);
};
