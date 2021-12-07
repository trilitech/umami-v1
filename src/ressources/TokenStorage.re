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
  include LocalStorage.Make({
    open RegisteredTokens;
    type t = RegisteredTokens.t;

    let key = "registered-tokens";

    let contractDecoder =
      Json.Decode.(field("contract", TokenContract.Decode.record));

    let chainDecoder = Json.Decode.(field("chain", string));

    let holderDecoder = Json.Decode.(field("holder", PublicKeyHash.decoder));
    let hiddenDecoder = Json.Decode.(field("hidden", bool));
    let balanceDecoder = json =>
      JsonEx.decode(
        json,
        Json.Decode.(
          field("balance", json => json |> string |> ReBigNumber.fromString)
        ),
      )
      ->Result.getWithDefault(ReBigNumber.fromInt(1));

    let nftInfoDecoder = json => {
      holder: holderDecoder(json),
      hidden: hiddenDecoder(json),
      balance: balanceDecoder(json),
    };

    let ftDecoder = _ => FT;
    let nftDecoder = json =>
      NFT(json |> Json.Decode.field("value", nftInfoDecoder));

    let kindDecoder = json =>
      Json.Decode.(
        json
        |> (
          field("kind", string)
          |> andThen(
               fun
               | "ft" => ftDecoder
               | "nft" => nftDecoder
               | v =>
                 JsonEx.(
                   raise(
                     InternalError(
                       DecodeError("Registered: Unknown kind " ++ v),
                     ),
                   )
                 ),
             )
        )
      );

    let tokensDecoder = json =>
      json
      |> Json.Decode.(
           either(int |> map(i => (i, FT)), tuple2(int, kindDecoder))
         );

    let registeredDecoder = json =>
      json
      |> Json.Decode.(field("tokens", array(tokensDecoder)))
      |> Map.Int.fromArray;

    module Decode = {
      let contract = json => {
        contract: contractDecoder(json),
        chain: chainDecoder(json),
        tokens: registeredDecoder(json),
      };

      let array = Json.Decode.array(contract);
    };

    module Encode = {
      let kindEncoder =
        Json.Encode.(
          fun
          | FT => object_([("kind", string("ft"))])
          | NFT(info) =>
            object_([
              ("kind", string("nft")),
              (
                "value",
                object_([
                  ("holder", info.holder |> PublicKeyHash.encoder),
                  ("hidden", info.hidden |> bool),
                  ("balance", info.balance |> ReBigNumber.toString |> string),
                ]),
              ),
            ])
        );

      let contract = c =>
        Json.Encode.(
          object_([
            ("contract", c.contract |> TokenContract.Encode.record),
            ("chain", c.chain |> string),
            (
              "tokens",
              c.tokens->Map.Int.toArray |> array(tuple2(int, kindEncoder)),
            ),
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

  let getWithFallback = () =>
    switch (get()) {
    | Error(LocalStorage.NotFound(_)) => Ok(PublicKeyHash.Map.empty)
    | res => res
    };
};

/** The cache is a representation of the already fetched tokens from the chain */
module Cache = {
  include LocalStorage.Make({
    type t = TokensLibrary.t;

    let key = "cached-tokens";

    let tokenEncoder = {
      Json.Encode.(
        fun
        | TokensLibrary.Token.Full(t) =>
          object_([
            ("kind", string("full")),
            ("value", t |> Token.Encode.record),
          ])
        | Partial(tc, bcd, retry) =>
          object_([
            ("kind", string("partial")),
            (
              "value",
              (tc, bcd, retry)
              |> tuple3(
                   TokenContract.Encode.record,
                   BCD.Encode.tokenBalanceEncoder,
                   bool,
                 ),
            ),
          ])
      );
    };

    let tokensEncoder = tks =>
      tks |> Map.Int.valuesToArray |> Json.Encode.array(tokenEncoder);

    let contractEncoder = (c: TokensLibrary.Generic.contract(_)) => {
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

    let partialValueDecoder = json => {
      Json.Decode.(
        json
        |> either(
             json =>
               json
               |> pair(
                    TokenContract.Decode.record,
                    BCD.Decode.tokenBalanceDecoder,
                  )
               |> (((tc, bcd)) => (tc, bcd, true)),
             tuple3(
               TokenContract.Decode.record,
               BCD.Decode.tokenBalanceDecoder,
               bool,
             ),
           )
      );
    };

    let partialDecoder = json => {
      open Json.Decode;
      let (tc, bcd, retry) = json |> field("value", partialValueDecoder);
      TokensLibrary.Token.Partial(tc, bcd, retry);
    };

    let fullDecoder = json =>
      TokensLibrary.Token.Full(
        json |> Json.Decode.field("value", Token.Decode.record),
      );

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
          tokens->Map.Int.set(TokensLibrary.Token.id(token), token)
        );

    let contractDecoder = json =>
      Json.Decode.(
        TokensLibrary.Generic.{
          name: json |> optional(field("name", string)),
          address: json |> field("address", PublicKeyHash.decoder),
          tokens: json |> field("tokens", tokensDecoder),
        }
      );

    let decoder = json => {
      let decoder =
        Json.Decode.(array(pair(PublicKeyHash.decoder, contractDecoder)));
      (json |> decoder)
      ->Array.reduce(PublicKeyHash.Map.empty, (cache, (pkh, tokens)) => {
          cache->PublicKeyHash.Map.set(pkh, tokens)
        });
    };
  });

  let getWithFallback = () =>
    switch (get()) {
    | Error(LocalStorage.NotFound(_)) => Ok(PublicKeyHash.Map.empty)
    | res => res
    };
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
              RegisteredTokens.{
                contract: TokenContract.{address, kind: `KFA1_2},
                chain: token.chain,
                tokens: Map.Int.empty->Map.Int.set(0, FT),
              },
            )
          // there is no FA2 contract registration possible at this point
          | FA2(_) => registered
          };
        },
      );

    let version = Version.mk(1, 3);

    let migrateRegistered = () => {
      let mapValue = storageString => {
        let%Res json = storageString->JsonEx.parse;
        let%ResMap storage = json->JsonEx.decode(Storage.decoder);
        storage->makeRegisteredStorage;
      };
      Registered.migrate(
        ~previousKey=Storage.key,
        ~mapValue,
        ~default=PublicKeyHash.Map.empty,
        (),
      );
    };

    let migrateCache = () => {
      let mapValue = storageString => {
        let%Res json = storageString->JsonEx.parse;
        let%ResMap storage = json->JsonEx.decode(Storage.decoder);
        storage->Array.reduce(PublicKeyHash.Map.empty, (cache, token) =>
          cache->TokensLibrary.addToken(Full(token))
        );
      };
      Cache.migrate(
        ~previousKey=Storage.key,
        ~mapValue,
        ~default=PublicKeyHash.Map.empty,
        (),
      );
    };

    let mk = () => {
      let%Res () = migrateCache();
      let%ResMap () = migrateRegistered();
      ();
      /* Storage.remove(); */
    };
  };
};

let mergeAccountNFTs =
    (
      registered: RegisteredTokens.t,
      nfts: TokensLibrary.WithBalance.t,
      holder,
    )
    : RegisteredTokens.t => {
  open RegisteredTokens;

  let newNFT = balance => NFT({holder, hidden: false, balance});

  let findChain = tokens =>
    tokens
    ->Map.Int.findFirstBy((_, _) => true)
    ->Option.flatMap(((_, (t, _))) =>
        switch (t) {
        | TokensLibrary.Token.Full(t) => t.TokenRepr.chain->Some
        | _ => None
        }
      );

  let mergeTokens = (_tokenId, kind, token) =>
    switch (kind, token) {
    | (None, None)
    | (Some(FT), None) => Some(FT)

    // An NFT is no longer hold by the account
    | (Some(NFT(info)), None) => info.holder == holder ? None : kind

    // Newly added NFT
    | (None, Some((t, balance))) =>
      t->TokensLibrary.Token.isNFT ? Some(newNFT(balance)) : None

    // NFT already exists: we keep its hidden status
    | (Some(NFT(info)), Some((_, balance))) =>
      {...info, holder, balance}->NFT->Some
    | _ => kind
    };

  let mergeContracts = (address, registeredContract, cachedContract) =>
    switch (registeredContract, cachedContract) {
    | (None, None) => None

    | (None, Some(contract)) =>
      let tokens =
        contract.TokensLibrary.Generic.tokens
        ->Map.Int.reduce(Map.Int.empty, (tokens, key, (t, balance)) =>
            t->TokensLibrary.Token.isNFT
              ? tokens->Map.Int.set(key, newNFT(balance)) : tokens
          );
      contract.tokens
      ->findChain
      ->Option.map(chain =>
          {contract: TokenContract.{address, kind: `KFA2}, chain, tokens}
        );

    // prune NFTs
    | (Some(contract), None) =>
      let tokens =
        contract.tokens
        ->Map.Int.keep((_, kind) =>
            switch (kind) {
            | FT => true
            | NFT(info) => holder != info.holder
            }
          );
      tokens->Map.Int.isEmpty ? None : {...contract, tokens}->Some;

    // prune no longer possessed NFTs, adds the new ones
    | (Some(registeredContract), Some(cachedContract)) =>
      let tokens =
        Map.Int.merge(
          registeredContract.tokens,
          cachedContract.TokensLibrary.Generic.tokens,
          mergeTokens,
        );
      {...registeredContract, tokens}->Some;
    };

  PublicKeyHash.Map.merge(registered, nfts, mergeContracts);
};
