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

open Let;

module Registered = {
  /** Representation of registered tokens for a user */
  type contract = {
    contract: TokenContract.t,
    chain: string,
    tokens: Set.Int.t // effectively registered tokens by the user
  };

  let isRegistered = (registered, pkh, tokenId) => {
    registered
    ->PublicKeyHash.Map.get(pkh)
    ->Option.map(t => t.tokens->Set.Int.has(tokenId))
    != None;
  };

  let registerToken = (registered, token: Token.t) => {
    registered->PublicKeyHash.Map.update(
      token.address,
      fun
      | None =>
        Some({
          contract:
            TokenContract.{
              address: token.address,
              kind: TokenContract.fromTokenKind(token.kind),
            },
          chain: token.chain,
          tokens: Set.Int.empty->Set.Int.add(TokenRepr.id(token)),
        })
      | Some(t) =>
        Some({...t, tokens: t.tokens->Set.Int.add(TokenRepr.id(token))}),
    );
  };

  let removeToken = (registered, pkh, tokenId) =>
    registered->PublicKeyHash.Map.update(
      pkh,
      fun
      | None => None
      | Some(t) => {
          let tokens = t.tokens->Set.Int.remove(tokenId);
          tokens->Set.Int.isEmpty ? None : Some({...t, tokens});
        },
    );

  include LocalStorage.Make({
    type t = PublicKeyHash.Map.map(contract);

    let key = "registered-tokens";

    let contractDecoder =
      Json.Decode.(field("contract", TokenContract.Decode.record));

    let chainDecoder = Json.Decode.(field("chain", string));

    let registeredDecoder = json =>
      json |> Json.Decode.(field("tokens", array(int))) |> Set.Int.fromArray;

    module Decode = {
      let contract = json => {
        contract: contractDecoder(json),
        chain: chainDecoder(json),
        tokens: registeredDecoder(json),
      };

      let array = Json.Decode.array(contract);
    };

    module Encode = {
      let contract = c =>
        Json.Encode.(
          object_([
            ("contract", c.contract |> TokenContract.Encode.record),
            ("chain", c.chain |> string),
            ("tokens", c.tokens->Set.Int.toArray |> array(int)),
          ])
        );

      let array = Json.Encode.array(contract);
    };

    let encoder = c => c->PublicKeyHash.Map.valuesToArray->Encode.array;
    let decoder = json => {
      json
      ->Decode.array
      ->Array.reduce(PublicKeyHash.Map.empty, (cache, token) =>
          cache->PublicKeyHash.Map.set(
            token.contract.TokenContract.address,
            token,
          )
        );
    };
  });
};

/** The cache is a representation of the already fetched tokens from the chain */
module Cache = {
  type token =
    | Full(Token.t)
    | Partial(TokenContract.t, BCD.tokenBalance);

  type tokens = Map.Int.t(token);

  type contract = {
    address: PublicKeyHash.t,
    name: option(string),
    tokens,
  };

  let empty = PublicKeyHash.Map.empty;

  let tokenId =
    fun
    | Full(t) => TokenRepr.id(t)
    | Partial(_, bcd) => bcd.BCD.token_id;

  let tokenAddress =
    fun
    | Full(t) => t.TokenRepr.address
    | Partial(_, bcd) => bcd.BCD.contract;

  let tokenKind =
    fun
    | Full(t) => TokenContract.fromTokenKind(t.TokenRepr.kind)
    | Partial(tc, _) => tc.TokenContract.kind;

  let tokenChain =
    fun
    | Full(t) => Some(t.TokenRepr.chain)
    | Partial(_, bcd) =>
      bcd.BCD.network->Network.networkChain->Option.map(Network.getChainId);

  let getToken = (cache, pkh, tokenId) => {
    cache
    ->PublicKeyHash.Map.get(pkh)
    ->Option.flatMap(({tokens}) => tokens->Map.Int.get(tokenId));
  };

  let isFull =
    fun
    | Full(_) => true
    | Partial(_, _) => false;

  let getFullToken = (cache, pkh, tokenId) => {
    switch (cache->getToken(pkh, tokenId)) {
    | Some(Full(t)) => Some(t)
    | _ => None
    };
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

  let addToken = (cache, token) =>
    updateToken(cache, tokenAddress(token), tokenId(token), ~updatedValue=_ =>
      Some(token)
    );

  let removeToken = (cache, token) => {
    let f = contract => {
      let tokens =
        contract
        ->Option.mapWithDefault(Map.Int.empty, c => c.tokens)
        ->Map.Int.remove(tokenId(token));
      contract->Option.map(contract => {...contract, tokens});
    };
    cache->PublicKeyHash.Map.update(tokenAddress(token), f);
  };

  let valuesToArray = cache => {
    cache
    ->PublicKeyHash.Map.valuesToArray
    ->Array.map(c => c.tokens->Map.Int.valuesToArray)
    ->Array.concatMany;
  };

  let merge = (cache1, cache2) => {
    let pickToken = (t1, t2) =>
      switch (t1, t2) {
      | (Full(_), Full(_)) => t1
      | (Full(_) as t, Partial(_, _))
      | (Partial(_, _), Full(_) as t) => t
      | _ => t1
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
    ();
    PublicKeyHash.Map.merge(cache1, cache2, mergeContracts);
  };

  include LocalStorage.Make({
    type t = PublicKeyHash.Map.map(contract);

    let key = "cached-tokens";

    let tokenEncoder = {
      Json.Encode.(
        fun
        | Full(t) =>
          object_([
            ("kind", string("full")),
            ("value", t |> Token.Encode.record),
          ])
        | Partial(tc, bcd) =>
          object_([
            ("kind", string("partial")),
            (
              "value",
              (tc, bcd)
              |> pair(
                   TokenContract.Encode.record,
                   BCD.Encode.tokenBalanceEncoder,
                 ),
            ),
          ])
      );
    };

    let tokensEncoder = tks =>
      tks |> Map.Int.valuesToArray |> Json.Encode.array(tokenEncoder);

    let contractEncoder = c => {
      Json.Encode.(
        object_([
          ("name", c.name |> nullable(string)),
          ("address", c.address |> PublicKeyHash.encoder),
          ("tokens", c.tokens |> tokensEncoder),
        ])
      );
    };

    let encoder = c =>
      c
      |> PublicKeyHash.Map.toArray
      |> Json.Encode.(array(pair(PublicKeyHash.encoder, contractEncoder)));

    let partialDecoder = json => {
      open Json.Decode;
      let (tc, bcd) =
        json
        |> field(
             "value",
             pair(
               TokenContract.Decode.record,
               BCD.Decode.tokenBalanceDecoder,
             ),
           );
      Partial(tc, bcd);
    };

    let fullDecoder = json =>
      Full(json |> Json.Decode.field("value", Token.Decode.record));

    let tokenDecoder = json => {
      Json.Decode.(
        json
        |> (
          field("kind", string)
          |> andThen(
               fun
               | "full" => fullDecoder
               | "partial" => partialDecoder
               | v =>
                 JsonEx.(
                   raise(
                     InternalError(DecodeError("Cache: Unknown kind " ++ v)),
                   )
                 ),
             )
        )
      );
    };

    let tokensDecoder = json =>
      (json |> Json.Decode.array(tokenDecoder))
      ->Array.reduce(Map.Int.empty, (tokens, token) =>
          tokens->Map.Int.set(tokenId(token), token)
        );

    let contractDecoder = json =>
      Json.Decode.{
        name: json |> optional(field("name", string)),
        address: json |> field("address", PublicKeyHash.decoder),
        tokens: json |> field("tokens", tokensDecoder),
      };

    let decoder = json => {
      let decoder =
        Json.Decode.(array(pair(PublicKeyHash.decoder, contractDecoder)));
      (json |> decoder)
      ->Array.reduce(PublicKeyHash.Map.empty, (cache, (pkh, tokens)) => {
          cache->PublicKeyHash.Map.set(pkh, tokens)
        });
    };
  });
};

module Legacy = {
  module V1_3 = {
    module Storage =
      LocalStorage.Make({
        type t = array(Token.t);
        let key = "wallet-tokens";
        let encoder = Token.Encode.array;
        let decoder = Token.Decode.array;
      });

    let makeRegisteredStorage = storage =>
      storage->Array.reduceReverse(
        PublicKeyHash.Map.empty,
        (registered, token) => {
          let address = token.TokenRepr.address;
          switch (token.TokenRepr.kind) {
          | FA1_2 =>
            registered->PublicKeyHash.Map.set(
              address,
              Registered.{
                contract: TokenContract.{address, kind: `KFA1_2},
                chain: token.chain,
                tokens: Set.Int.empty->Set.Int.add(0),
              },
            )
          // there is no FA2 contract registration possible at this point
          | FA2(_) => registered
          };
        },
      );

    let version = Version.mk(1, 3);

    let migrateRegistered = () => {
      let mapValue = _ => {
        let%ResMap storage = Storage.get();
        storage->makeRegisteredStorage;
      };
      Registered.migrate(~mapValue, ~default=PublicKeyHash.Map.empty, ());
    };

    let migrateCache = () => {
      let mapValue = _ => {
        let%ResMap storage = Storage.get();
        Js.log(storage);
        storage->Array.reduce(PublicKeyHash.Map.empty, (cache, token) =>
          cache->Cache.addToken(Full(token))
        );
      };
      Cache.migrate(~mapValue, ~default=PublicKeyHash.Map.empty, ());
    };

    let mk = () => {
      let%Res () = migrateCache();
      let%ResMap () = migrateRegistered();
      ();
      /* Storage.remove(); */
    };
  };
};
