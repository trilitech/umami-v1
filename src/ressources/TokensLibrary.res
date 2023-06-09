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

module Token = {
  type t =
    | Full(TokenRepr.t)
    | Partial(Tzkt.t, Network.chain<Network.chainId>, bool)

  let id = x =>
    switch x {
    | Full(t) => TokenRepr.id(t)
    | Partial(t, _, _) => t->Tzkt.tokenId
    }

  let address = x =>
    switch x {
    | Full(t) => t.TokenRepr.address
    | Partial(t, _, _) => t->Tzkt.address
    }

  let name = x =>
    switch x {
    | Full(t) => t.TokenRepr.alias->Some
    | Partial(t, _, _) => t.Tzkt.tokenInfo.metadata->Option.flatMap(m => m.Tzkt.name)
    }

  let symbol = x =>
    switch x {
    | Full(t) => t.TokenRepr.symbol->Some
    | Partial(t, _, _) => t.Tzkt.tokenInfo.metadata->Option.flatMap(m => m.Tzkt.symbol)
    }

  let decimals = x =>
    switch x {
    | Full(t) => t.TokenRepr.decimals->Some
    | Partial(t, _, _) => t.Tzkt.tokenInfo.metadata->Option.flatMap(m => m.Tzkt.decimals)
    }

  let kind = x =>
    switch x {
    | Full(t) => TokenContract.fromTokenKind(t.TokenRepr.kind)
    | Partial(t, _, _) => t.Tzkt.tokenInfo.standard
    }

  let chain = x =>
    switch x {
    | Full(t) => t.TokenRepr.chain
    | Partial(_, chain, _) => chain->Network.getChainId
    }

  let isNFT = x =>
    switch x {
    | Full(t) => t->TokenRepr.isNFT
    | Partial(_, _, _) => false
    }

  let isFull = x =>
    switch x {
    | Full(_) => true
    | Partial(_, _, _) => false
    }

  let toTokenRepr = (~alias=?, ~symbol=?, ~decimals=?, t) =>
    switch t {
    | Full(t) =>
      {
        open TokenRepr
        {
          ...t,
          alias: alias->Option.default(t.alias),
          symbol: symbol->Option.default(t.symbol),
          decimals: decimals->Option.default(t.decimals),
        }
      }->Some
    | Partial(t, chain, _) =>
      open Tzkt
      let metadata = metadata => {...metadata, name: alias, symbol: symbol, decimals: decimals}
      let t = {
        ...t,
        tokenInfo: {
          ...t.tokenInfo,
          metadata: t.tokenInfo.metadata->Option.map(metadata),
        },
      }
      toTokenRepr(chain->Network.getChainId->Some, t)
    }

  let uniqueKey = t => (t->address :> string) ++ t->id->Int.toString
}

module Contracts = PublicKeyHash.Map
module Ids = Map.Int

module Generic = {
  type contract<'tokens> = {
    address: PublicKeyHash.t,
    name: option<string>,
    tokens: Ids.t<'tokens>,
  }

  type t<'token> = Contracts.map<contract<'token>>

  let empty = Contracts.empty

  let getToken = (cache, pkh, tokenId) =>
    cache->Contracts.get(pkh)->Option.flatMap(({tokens}) => tokens->Ids.get(tokenId))

  let updateToken = (cache, pkh, tokenId, ~updatedValue) => {
    let f = contract => {
      let tokens =
        contract->Option.mapWithDefault(Ids.empty, c => c.tokens)->Ids.update(tokenId, updatedValue)
      contract
      ->Option.mapWithDefault({name: None, address: pkh, tokens: tokens}, contract => {
        ...contract,
        tokens: tokens,
      })
      ->Some
    }
    cache->Contracts.update(pkh, f)
  }

  let map = (cache, f) => {
    let mapIds = ids => Ids.map(ids, f)
    cache->Contracts.map(c => {...c, tokens: c.tokens->mapIds})
  }

  let valuesToArray = cache =>
    cache->Contracts.valuesToArray->Array.map(c => c.tokens->Ids.valuesToArray)->Array.concatMany

  let reduce = (cache: t<'token>, acc: 'a, f: ('a, PublicKeyHash.t, int, 'token) => 'a): 'a =>
    cache->Contracts.reduce(acc, (acc, pkh, c) =>
      c.tokens->Ids.reduce(acc, (acc, id, t) => f(acc, pkh, id, t))
    )

  let keepMap = (cache, f) =>
    cache->Contracts.keepMap((pkh, c) => {
      let tokens = c.tokens->Map.keepMapInt((id, token) => f(pkh, id, token))
      tokens->Ids.isEmpty ? None : Some({...c, tokens: tokens})
    })

  let keepTokens = (cache, f) =>
    cache->Contracts.reduce(Contracts.empty, (cache, pkh, c) => {
      let tokens = c.tokens->Ids.keep((id, token) => f(pkh, id, token))
      tokens->Ids.isEmpty ? cache : cache->Contracts.set(pkh, {...c, tokens: tokens})
    })

  let keepPartition = (cache, f) =>
    cache->Contracts.reduce((Contracts.empty, Contracts.empty), ((left, right), pkh, c) => {
      let (leftTokens, rightTokens) = c.tokens->Ids.reduce((Ids.empty, Ids.empty), (
        (left, right),
        id,
        token,
      ) =>
        switch f(pkh, id, token) {
        | None => (left, right)
        | Some(true) => (left->Ids.set(id, token), right)
        | Some(false) => (left, right->Ids.set(id, token))
        }
      )
      let left =
        leftTokens->Ids.isEmpty ? left : left->Contracts.set(pkh, {...c, tokens: leftTokens})
      let right =
        rightTokens->Ids.isEmpty ? right : right->Contracts.set(pkh, {...c, tokens: rightTokens})
      (left, right)
    })

  let pickAnyFromContract = contract =>
    contract.tokens->Map.pickAnyInt->Option.map(((id, t)) => (contract.address, id, t))

  let pickAnyAtAddress = (cache, address) =>
    cache->Contracts.get(address)->Option.flatMap(pickAnyFromContract)

  let pickAny = cache =>
    cache->Contracts.pickAny->Option.flatMap(((_, c)) => c->pickAnyFromContract)
}

module WithBalance = {
  open Token
  open Generic

  type token = (Token.t, ReBigNumber.t)
  type contract = Generic.contract<token>
  type t = Generic.t<token>

  // map2 is always the newly added tokens
  let mergeAndUpdateBalance = (map1, map2) => {
    let pickToken = ((t1, b1), (t2, b2)) =>
      switch (t1, t2) {
      | (Full(_), Full(_)) => (t1, b2)
      | (Full(_) as t, Partial(_, _, _))
      | (Partial(_, _, _), Full(_) as t) => (t, b2)
      | _ => (t1, b1)
      }
    let mergeTokens = (_, t1, t2) =>
      switch (t1, t2) {
      | (None, None) => None
      | (Some(_) as t, None)
      | (None, Some(_) as t) => t
      | (Some(t1), Some(t2)) => pickToken(t1, t2)->Some
      }
    let mergeContracts = (key, c1, c2) =>
      switch (c1, c2) {
      | (None, None) => None
      | (Some(_) as c, None)
      | (None, Some(_) as c) => c
      | (Some(c1), Some(c2)) =>
        {
          address: key,
          name: Option.keep(c1.name, c2.name),
          tokens: Ids.merge(c1.tokens, c2.tokens, mergeTokens),
        }->Some
      }
    Contracts.merge(map1, map2, mergeContracts)
  }

  let getFullToken = (map, pkh, tokenId) =>
    switch map->getToken(pkh, tokenId) {
    | Some((Full(t), b)) => Some((t, b))
    | _ => None
    }
}

module WithRegistration = {
  open Token
  open Generic

  type token = (Token.t, bool)
  type contract = Generic.contract<token>
  type t = Generic.t<token>

  // map2 is always the newly added tokens
  let keepAndSetRegistration = (
    map1: Generic.t<'token>,
    map2: RegisteredTokens.t,
    extractToken: 'token => option<Token.t>,
  ): t => {
    let mergeTokens = (_, t, reg) =>
      switch t {
      | None => None
      | Some(t) => t->extractToken->Option.map(token => (token, reg != None))
      }
    let mergeContracts = (_, c1, c2) =>
      switch (c1, c2) {
      | (None, None)
      | (None, Some(_)) =>
        None
      | (Some(c), None) =>
        let tokens =
          c.tokens->Map.keepMapInt((_, t) => t->extractToken->Option.map(t => (t, false)))
        tokens->Ids.isEmpty ? None : {...c, tokens: tokens}->Some
      | (Some(c1), Some(c2: RegisteredTokens.contract)) =>
        let tokens = Ids.merge(c1.tokens, c2.tokens, mergeTokens)
        tokens->Ids.isEmpty ? None : {...c1, tokens: tokens}->Some
      }
    Contracts.merge(map1, map2, mergeContracts)
  }

  let getFullToken = (map, pkh, tokenId) =>
    switch map->getToken(pkh, tokenId) {
    | Some((Full(t), reg)) => Some((t, reg))
    | _ => None
    }
}

type t = Generic.t<Token.t>

let getFullToken = (map, pkh, tokenId) =>
  switch map->Generic.getToken(pkh, tokenId) {
  | Some(Token.Full(t)) => Some(t)
  | _ => None
  }

let addToken = (map, token) =>
  Generic.updateToken(map, Token.address(token), Token.id(token), ~updatedValue=_ => Some(token))

let removeToken = (map, token) => {
  let f = contract => {
    let tokens =
      contract->Option.mapWithDefault(Ids.empty, c => c.Generic.tokens)->Ids.remove(Token.id(token))
    contract->Option.map(contract => {...contract, tokens: tokens})
  }
  map->Contracts.update(Token.address(token), f)
}

let invalidateCache = (cache, filter) => {
  let invalidate = (token: Token.t) =>
    switch token {
    | Full(token) =>
      Token.Partial(token->Tzkt.fromTokenRepr, token.chain->Network.fromChainId, true)
    | _ => token
    }
  let map = (token: Token.t) =>
    switch filter {
    | #Any => token->invalidate
    | #FT => token->Token.isNFT ? token : token->invalidate
    | #NFT => token->Token.isNFT ? token->invalidate : token
    }
  cache->Generic.map(map)
}

let forceRetryPartial = (cache, filter) => {
  let forceRetry = (token: Token.t) =>
    switch token {
    | Partial(tc, bcd, _) => Some(Token.Partial(tc, bcd, true))
    | _ => Some(token)
    }
  let map = (_, _, token: Token.t) =>
    switch filter {
    | #Any => token->forceRetry
    | #FT => token->Token.isNFT ? Some(token) : token->forceRetry
    | #NFT => token->Token.isNFT ? token->forceRetry : Some(token)
    }
  cache->Generic.keepMap(map)
}
