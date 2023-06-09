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

module Registered = {
  include LocalStorage.Make({
    open RegisteredTokens
    type t = RegisteredTokens.t

    let key = "registered-tokens"

    let contractDecoder = {
      open Json.Decode
      field("contract", TokenContract.Decode.record)
    }

    let chainDecoder = {
      open Json.Decode
      field("chain", Network.Decode.chainIdDecoder)
    }

    let holderDecoder = {
      open Json.Decode
      field("holder", PublicKeyHash.decoder)
    }
    let hiddenDecoder = {
      open Json.Decode
      field("hidden", bool)
    }
    let balanceDecoder = json =>
      JsonEx.decode(
        json,
        {
          open Json.Decode
          field("balance", json => json |> string |> ReBigNumber.fromString)
        },
      )->Result.getWithDefault(ReBigNumber.fromInt(1))

    let nftInfoDecoder = json => {
      holder: holderDecoder(json),
      hidden: hiddenDecoder(json),
      balance: balanceDecoder(json),
    }

    let ftDecoder = _ => FT
    let nftDecoder = json => NFT(json |> Json.Decode.field("value", nftInfoDecoder))

    let kindDecoder = json => {
      open Json.Decode
      json
      |> (field("kind", string)
      |> andThen(x =>
        switch x {
        | "ft" => ftDecoder
        | "nft" => nftDecoder
        | v => raise(Json.Decode.DecodeError("Registered: Unknown kind " ++ v))
        }
      ))
    }

    let tokensDecoder = json =>
      json |> {
        open Json.Decode
        either(int |> map(i => (i, FT)), tuple2(int, kindDecoder))
      }

    let registeredDecoder = json =>
      json
      |> {
        open Json.Decode
        field("tokens", array(tokensDecoder))
      }
      |> Map.Int.fromArray

    module Decode = {
      let contract = json => {
        contract: contractDecoder(json),
        chain: chainDecoder(json),
        tokens: registeredDecoder(json),
      }

      let array = Json.Decode.array(contract)
    }

    module Encode = {
      let kindEncoder = {
        open Json.Encode

        x =>
          switch x {
          | FT => object_(list{("kind", string("ft"))})
          | NFT(info) =>
            object_(list{
              ("kind", string("nft")),
              (
                "value",
                object_(list{
                  ("holder", info.holder |> PublicKeyHash.encoder),
                  ("hidden", info.hidden |> bool),
                  ("balance", info.balance |> ReBigNumber.toString |> string),
                }),
              ),
            })
          }
      }

      let contract = c => {
        open Json.Encode
        object_(list{
          ("contract", c.contract |> TokenContract.Encode.record),
          ("chain", c.chain |> Network.Encode.chainIdEncoder),
          ("tokens", c.tokens->Map.Int.toArray |> array(tuple2(int, kindEncoder))),
        })
      }

      let array = Json.Encode.array(contract)
    }

    let encoder = c => c->PublicKeyHash.Map.valuesToArray->Encode.array
    let decoder = json =>
      json
      ->Decode.array
      ->Array.reduce(PublicKeyHash.Map.empty, (cache, token) =>
        cache->PublicKeyHash.Map.set(token.contract.TokenContract.address, token)
      )
  })

  let getWithFallback = () =>
    switch get() {
    | Error(LocalStorage.NotFound(_)) => Ok(PublicKeyHash.Map.empty)
    | res => res
    }
}

@ocaml.doc(" The cache is a representation of the already fetched tokens from the chain ")
module Cache = {
  module StorageRepr = {
    type t = TokensLibrary.t

    let key = "cached-tokens"

    let tokenEncoder = {
      open Json.Encode

      x =>
        switch x {
        | TokensLibrary.Token.Full(t) =>
          object_(list{("kind", string("full")), ("value", t |> Token.Encode.record)})
        | Partial(t, chain, retry) =>
          object_(list{
            ("kind", string("partial")),
            (
              "value",
              (t, chain, retry) |> tuple3(Tzkt.Encode.encoder, Network.Encode.chainEncoder, bool),
            ),
          })
        }
    }

    let tokensEncoder = tks => tks |> Map.Int.valuesToArray |> Json.Encode.array(tokenEncoder)

    let contractEncoder = (c: TokensLibrary.Generic.contract<_>) => {
      open Json.Encode
      object_(list{
        ("name", c.name |> nullable(string)),
        ("address", c.address |> PublicKeyHash.encoder),
        ("tokens", c.tokens |> tokensEncoder),
      })
    }

    let encoder = c =>
      c
      |> PublicKeyHash.Map.toArray
      |> {
        open Json.Encode
        array(pair(PublicKeyHash.encoder, contractEncoder))
      }

    let partialValueDecoder = json => {
      open Json.Decode
      json |> either(
        json =>
          json
          |> pair(
            Tzkt.Decode.decoder,
            {
              open Network.Decode
              chainDecoder(chainFromString)
            },
          )
          |> (((t, chain)) => (t, chain, true)),
        tuple3(
          Tzkt.Decode.decoder,
          {
            open Network.Decode
            chainDecoder(chainFromString)
          },
          bool,
        ),
      )
    }

    let partialDecoder = (purgePartial, json) => {
      open Json.Decode
      if purgePartial {
        None
      } else {
        let (tc, bcd, retry) = json |> field("value", partialValueDecoder)
        TokensLibrary.Token.Partial(tc, bcd, retry)->Some
      }
    }

    let fullDecoder = json =>
      TokensLibrary.Token.Full(json |> Json.Decode.field("value", Token.Decode.record))->Some

    let tokenDecoder = (purgePartial, json) => {
      open Json.Decode
      json
      |> (field("kind", string)
      |> andThen(x =>
        switch x {
        | "full" => fullDecoder
        | "partial" => partialDecoder(purgePartial)
        | v => raise(Json.Decode.DecodeError("Cache: Unknown kind " ++ v))
        }
      ))
    }

    let tokensDecoder = (purgePartial, json) =>
      (json |> Json.Decode.array(tokenDecoder(purgePartial)))
        ->Array.reduce(Map.Int.empty, (tokens, token) =>
          switch token {
          | Some(token) => tokens->Map.Int.set(TokensLibrary.Token.id(token), token)
          | None => tokens
          }
        )

    let contractDecoder = (purgePartial, json) => {
      open Json.Decode
      open TokensLibrary.Generic
      {
        name: json |> optional(field("name", string)),
        address: json |> field("address", PublicKeyHash.decoder),
        tokens: json |> field("tokens", tokensDecoder(purgePartial)),
      }
    }

    let decoderRaw = (purgePartial, json) => {
      let decoder = {
        open Json.Decode
        array(pair(PublicKeyHash.decoder, contractDecoder(purgePartial)))
      }
      (json |> decoder)
        ->Array.reduce(PublicKeyHash.Map.empty, (cache, (pkh, tokens)) =>
          cache->PublicKeyHash.Map.set(pkh, tokens)
        )
    }

    let decoder = decoderRaw(false)
    let decoderWithoutPartial = decoderRaw(true)
  }

  include LocalStorage.Make(StorageRepr)

  let getWithFallback = () =>
    switch get() {
    | Error(LocalStorage.NotFound(_)) => Ok(PublicKeyHash.Map.empty)
    | res => res
    }
}

module Legacy = {
  module V1_3 = {
    module Storage = LocalStorage.Make({
      type t = array<Token.t>
      let key = "wallet-tokens"
      let encoder = Token.Encode.array
      let decoder = Token.Decode.array
    })

    let makeRegisteredStorage = storage =>
      storage->Array.reduceReverse(PublicKeyHash.Map.empty, (registered, token) => {
        let address = token.TokenRepr.address
        switch token.TokenRepr.kind {
        | FA1_2 =>
          registered->PublicKeyHash.Map.set(
            address,
            {
              open RegisteredTokens
              {
                contract: {
                  open TokenContract
                  {address: address, kind: #KFA1_2}
                },
                chain: token.chain,
                tokens: Map.Int.empty->Map.Int.set(0, FT),
              }
            },
          )
        // there is no FA2 contract registration possible at this point
        | FA2(_) => registered
        }
      })

    let version = Version.mk(1, 3)

    let migrateRegistered = () => {
      let mapValue = storageString =>
        storageString
        ->JsonEx.parse
        ->Result.flatMap(json => json->JsonEx.decode(Storage.decoder))
        ->Result.map(makeRegisteredStorage)
      Registered.migrate(~previousKey=Storage.key, ~mapValue, ~default=PublicKeyHash.Map.empty, ())
    }

    let migrateCache = () => {
      let mapValue = storageString =>
        storageString
        ->JsonEx.parse
        ->Result.flatMap(json => json->JsonEx.decode(Storage.decoder))
        ->Result.map(storage =>
          storage->Array.reduce(PublicKeyHash.Map.empty, (cache, token) =>
            cache->TokensLibrary.addToken(Full(token))
          )
        )
      Cache.migrate(~previousKey=Storage.key, ~mapValue, ~default=PublicKeyHash.Map.empty, ())
    }

    let mk = () => migrateCache()->Result.flatMap(migrateRegistered)
  }

  module V1_4 = {
    let version = Version.mk(1, 4)

    let mk = () => {
      let mapValue = cacheString =>
        cacheString
        ->JsonEx.parse
        ->Result.flatMap(json => json->JsonEx.decode(Cache.decoder))
        ->Result.map(cache => cache->TokensLibrary.invalidateCache(#NFT))
      Cache.migrate(~default=TokensLibrary.Generic.empty, ~mapValue, ())
    }
  }

  module V1_6 = {
    let version = Version.mk(1, 6)

    let mk = () => {
      let mapValue = cacheString =>
        cacheString
        ->JsonEx.parse
        ->Result.flatMap(json => json->JsonEx.decode(Cache.decoder))
        ->Result.map(cache => cache->TokensLibrary.forceRetryPartial(#NFT))
      Cache.migrate(~default=TokensLibrary.Generic.empty, ~mapValue, ())
    }
  }

  module V1_7 = {
    let version = Version.mk(1, 7)
    let mk = () => {
      let mapValue = cacheString =>
        cacheString
        ->JsonEx.parse
        ->Result.flatMap(json => json->JsonEx.decode(Cache.StorageRepr.decoderWithoutPartial))
      Cache.migrate(~default=TokensLibrary.Generic.empty, ~mapValue, ())
    }
  }
}

let mergeAccountNFTs = (
  registered: RegisteredTokens.t,
  nfts: TokensLibrary.WithBalance.t,
  holder,
): RegisteredTokens.t => {
  open RegisteredTokens

  let newNFT = balance => NFT({holder: holder, hidden: false, balance: balance})

  let findChain = tokens =>
    tokens
    ->Map.Int.findFirstBy((_, _) => true)
    ->Option.flatMap(((_, (t, _))) =>
      switch t {
      | TokensLibrary.Token.Full(t) => t.TokenRepr.chain->Some
      | _ => None
      }
    )

  let mergeTokens = (_tokenId, kind, token) =>
    switch (kind, token) {
    | (None, None)
    | (Some(FT), None) =>
      Some(FT)

    // An NFT is no longer hold by the account
    | (Some(NFT(info)), None) => info.holder == holder ? None : kind

    // Newly added NFT
    | (None, Some((t, balance))) => t->TokensLibrary.Token.isNFT ? Some(newNFT(balance)) : None

    // NFT already exists: we keep its hidden status
    | (Some(NFT(info)), Some((_, balance))) =>
      {...info, holder: holder, balance: balance}->NFT->Some
    | _ => kind
    }

  let mergeContracts = (address, registeredContract, cachedContract) =>
    switch (registeredContract, cachedContract) {
    | (None, None) => None

    | (None, Some(contract)) =>
      let tokens =
        contract.TokensLibrary.Generic.tokens->Map.Int.reduce(Map.Int.empty, (
          tokens,
          key,
          (t, balance),
        ) => t->TokensLibrary.Token.isNFT ? tokens->Map.Int.set(key, newNFT(balance)) : tokens)
      contract.tokens
      ->findChain
      ->Option.map(chain => {
        contract: {
          open TokenContract
          {address: address, kind: #KFA2}
        },
        chain: chain,
        tokens: tokens,
      })

    // prune NFTs
    | (Some(contract), None) =>
      let tokens = contract.tokens->Map.Int.keep((_, kind) =>
        switch kind {
        | FT => true
        | NFT(info) => holder != info.holder
        }
      )
      tokens->Map.Int.isEmpty ? None : {...contract, tokens: tokens}->Some

    // prune no longer possessed NFTs, adds the new ones
    | (Some(registeredContract), Some(cachedContract)) =>
      let tokens = Map.Int.merge(
        registeredContract.tokens,
        cachedContract.TokensLibrary.Generic.tokens,
        mergeTokens,
      )
      {...registeredContract, tokens: tokens}->Some
    }

  PublicKeyHash.Map.merge(registered, nfts, mergeContracts)
}
