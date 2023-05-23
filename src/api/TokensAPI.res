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

let () = Errors.registerHandler("Tokens", x =>
  switch x {
  | NotFAContract(_) => I18n.error_check_contract->Some
  | RegisterNotAFungibleToken(_) => I18n.error_register_not_fungible->Some
  | RegisterNotANonFungibleToken(_) => I18n.error_register_not_non_fungible->Some
  | _ => None
  }
)

module TzktAPI = {
  open Tzkt

  let mergeBalanceArray = (fetched, balances) => {
    let add = (map, token) => {
      let f = tokens =>
        tokens->Option.getWithDefault(Map.Int.empty)->Map.Int.set(token->tokenId, token)->Some
      map->PublicKeyHash.Map.update(token->address, f)
    }

    balances->Array.reduce(fetched, add)
  }

  let tokensNumber = tokens =>
    tokens->PublicKeyHash.Map.reduce(0, (total, _, ids) => ids->Map.Int.size + total)

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let fetchTokens = (network, alreadyFetched, account, index, number, total) => {
    let rec fetch = (alreadyFetched, index, number) =>
      ServerAPI.URL.External.tzktAccountTokens(
        ~network,
        ~account,
        ~sortBy=#Contract,
        ~limit=min(requestPageSize, number),
        ~index,
        ~hideEmpty=true,
        (),
      )
      ->Promise.value
      ->Promise.flatMapOk(url => url->ServerAPI.URL.get)
      ->Promise.flatMapOk(json => json->JsonEx.decode(Decode.arrayDecoder)->Promise.value)
      ->Promise.flatMapOk(tokens => {
        // corresponds to the number of tokens to fetch according to
        // the number we ask, not the total remaining
        let remaining = max(number - requestPageSize, 0)

        // index for the next fetch
        let index = index + tokens->Array.size

        // Final index
        let finalIndex = min(index, total)

        let remainingOnTotal = max(total - index, 0)

        // adds the newly fetched tokens to the cache currently built
        let alreadyFetched = alreadyFetched->mergeBalanceArray(tokens)

        remaining <= 0 || remainingOnTotal <= 0
          ? (alreadyFetched, finalIndex)->Promise.ok
          : fetch(alreadyFetched, index, remaining)
      })
    fetch(alreadyFetched, index, number)
  }

  let fetchTokensNumber = (network, account) =>
    ServerAPI.URL.External.tzktAccountTokensNumber(~network, ~account)
    ->FutureBase.value
    ->Promise.flatMapOk(url => ServerAPI.URL.get(url))
    ->Promise.flatMapOk(number => number->JsonEx.decode(Json.Decode.int)->Promise.value)

  // fetch a certain number of token for each account
  let fetchAccountsTokens = (config, accounts, index, numberByAccount) => {
    let rec fetch = (alreadyFetched, accounts, highestIndex, totalAccumulated) =>
      switch accounts {
      // the highest index corresponds to the accounts that has the most of
      // tokens. This is useful to know if this is the last page to fetch
      // (highestIndex < index) or if there are more tokens on at least one
      // account
      | list{} => (alreadyFetched, highestIndex, totalAccumulated)->Promise.ok
      | list{account, ...accounts} =>
        fetchTokensNumber(config, account)->Promise.flatMapOk(total =>
          fetchTokens(
            config,
            alreadyFetched,
            account,
            index,
            numberByAccount,
            total,
          )->Promise.flatMapOk(((fetched, index)) =>
            fetch(fetched, accounts, max(index, highestIndex), total + totalAccumulated)
          )
        )
      }

    fetch(PublicKeyHash.Map.empty, accounts, index, 0)
  }

}

type filter = [#Any | #FT | #NFT(PublicKeyHash.t, bool)]

/* From a list of tokens and the cache, reconstructs the list of tokens with
 their metadata the user has registered */
let unfoldRegistered = (tokens, cache: TokensLibrary.t, filter): TokensLibrary.WithBalance.t => {
  let mergeTokens = (_, registeredToken, cachedToken) =>
    switch (registeredToken, cachedToken) {
    | (None, _)
    | (Some(_), None) =>
      None
    | (Some(registered), Some(cached)) =>
      switch (registered, filter) {
      | (RegisteredTokens.NFT(info), #Any) => Some((cached, info.balance))
      | (FT, #FT | #Any) => Some((cached, ReBigNumber.fromInt(0)))
      | (NFT(info), #NFT(holder, allowHidden)) =>
        (!allowHidden ? !info.hidden : true) && info.holder == holder
          ? Some((cached, info.balance))
          : None
      | _ => None
      }
    }

  let merge = (_, registered, tokens) =>
    switch (registered, tokens) {
    | (None, _)
    // What should we do if the token is not in cache?
    | (_, None) =>
      None
    | (Some(registeredContract), Some(cachedContract)) =>
      let tokens = Map.Int.merge(
        registeredContract.RegisteredTokens.tokens,
        cachedContract.TokensLibrary.Generic.tokens,
        mergeTokens,
      )

      tokens->Map.Int.isEmpty ? None : Some({...cachedContract, tokens: tokens})
    }
  PublicKeyHash.Map.merge(tokens, cache, merge)
}

let registeredTokens = filter =>
  TokenStorage.Registered.getWithFallback()->Result.flatMap(tokens =>
    TokenStorage.Cache.getWithFallback()->Result.map(cache =>
      tokens->unfoldRegistered(cache, filter)
    )
  )

let hiddenTokens = () =>
  TokenStorage.Registered.getWithFallback()->Result.map(tokens =>
    tokens->RegisteredTokens.keepTokens((_, _, x) =>
      switch x {
      | FT => false
      | NFT({hidden}) => hidden
      }
    )
  )

// used for registration of custom tokens
let addTokenToCache = (config, token) => {
  let address = TokensLibrary.Token.address(token)
  config
  ->NodeAPI.Tokens.checkTokenContract(address)
  ->Promise.flatMapOk(tokenKind =>
    switch tokenKind {
    | #...TokenContract.kind => Promise.ok()
    | _ => Promise.err(NotFAContract((address :> string)))
    }
  )
  ->Promise.mapOk(() => {
    let tokens =
      TokenStorage.Cache.getWithFallback()
      ->Result.getWithDefault(PublicKeyHash.Map.empty)
      ->TokensLibrary.addToken(token)

    TokenStorage.Cache.set(tokens)
  })
}

let addTokenToRegistered = (token, kind) => {
  let tokens =
    TokenStorage.Registered.getWithFallback()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->RegisteredTokens.registerToken(token, kind)

  TokenStorage.Registered.set(tokens)
}

let registerNFTs = (tokens, holder) =>
  TokenStorage.Registered.getWithFallback()->Result.map(registered =>
    registered->TokenStorage.mergeAccountNFTs(tokens, holder)->TokenStorage.Registered.set
  )

let updateNFTsVisibility = (updatedTokens, ~hidden) =>
  TokenStorage.Registered.getWithFallback()->Result.map(registered => {
    let registered = registered->RegisteredTokens.updateNFTsVisibility(updatedTokens, hidden)
    registered->TokenStorage.Registered.set
    registered
  })

let addTokenToCache = (config, token) => addTokenToCache(config, Full(token))

let addFungibleToken = (config, token) =>
  (
    token->TokenRepr.isNFT
      ? Promise.err(RegisterNotAFungibleToken(token.address, token.kind))
      : Promise.ok()
  )
  ->Promise.flatMapOk(() => addTokenToCache(config, token))
  ->Promise.mapOk(() => addTokenToRegistered(token, RegisteredTokens.FT))

let addNonFungibleToken = (config, token, holder, balance) =>
  (
    token->TokenRepr.isNFT
      ? Promise.ok()
      : Promise.err(RegisterNotANonFungibleToken(token.address, token.kind))
  )
  ->Promise.flatMapOk(() => addTokenToCache(config, token))
  ->Promise.mapOk(() =>
    addTokenToRegistered(
      token,
      {
        open RegisteredTokens
        NFT({holder: holder, hidden: false, balance: balance})
      },
    )
  )

let removeFromCache = token =>
  TokenStorage.Cache.getWithFallback()->Result.map(tokens =>
    tokens->TokensLibrary.removeToken(token)->TokenStorage.Cache.set
  )

let removeFromRegistered = (token: Token.t) =>
  TokenStorage.Registered.getWithFallback()->Result.map(tokens =>
    TokenStorage.Registered.set(
      tokens->RegisteredTokens.removeToken(token.address, TokenRepr.id(token)),
    )
  )

let removeToken = (token, ~pruneCache) =>
  (pruneCache ? removeFromCache(Full(token)) : Ok())->Result.flatMap(() =>
    removeFromRegistered(token)
  )

let metadataToAsset = (metadata: ReTaquitoTypes.Tzip12.metadata) => {
  open Metadata
  {
    description: metadata.description,
    minter: metadata.minter,
    creators: metadata.creators,
    contributors: metadata.contributors,
    publishers: metadata.publishers,
    date: metadata.date,
    blocklevel: metadata.blocklevel,
    type_: metadata.type_,
    tags: metadata.tags,
    genres: metadata.genres,
    language: metadata.language,
    identifier: metadata.identifier,
    rights: metadata.rights,
    rightUri: metadata.rightUri,
    artifactUri: metadata.artifactUri,
    displayUri: metadata.displayUri,
    thumbnailUri: Metadata.thumbnailUriFromFormat(metadata.thumbnailUri, metadata.formats),
    isTransferable: metadata.isTransferable->Option.getWithDefault(true),
    isBooleanAmount: metadata.isBooleanAmount->Option.getWithDefault(false),
    shouldPreferSymbol: metadata.shouldPreferSymbol->Option.getWithDefault(false),
    formats: metadata.formats,
    attributes: metadata.attributes,
  }
}

let metadataToToken = (
  chain: Network.chainId,
  tokenContract: TokenContract.t,
  metadata: ReTaquitoTypes.Tzip12.metadata,
) => {
  open TokenRepr
  {
    kind: TokenContract.toTokenKind(tokenContract.kind, metadata.token_id),
    address: tokenContract.address,
    alias: metadata.name,
    symbol: metadata.symbol,
    decimals: metadata.decimals,
    chain: chain,
    asset: metadata->metadataToAsset,
  }
}

let handleRegistrationStatus = (cache, keepMap) =>
  TokenStorage.Registered.getWithFallback()
  ->Promise.value
  ->Promise.mapOk(registered =>
    TokensLibrary.WithRegistration.keepAndSetRegistration(cache, registered, keepMap)
  )

let keepToken = (token, network: Network.t, filter) => {
  let kindOk = switch filter {
  | #Any => true
  | #FT => !(token->TokensLibrary.Token.isNFT)
  | #NFT => token->TokensLibrary.Token.isNFT
  }

  TokensLibrary.Token.chain(token) == network.chain->Network.getChainId && kindOk
}

let cachedTokensWithRegistration = (network, filter) =>
  TokenStorage.Cache.getWithFallback()
  ->Promise.value
  ->Promise.flatMapOk(cache =>
    cache->handleRegistrationStatus(token => token->keepToken(network, filter) ? Some(token) : None)
  )
  ->Promise.mapOk(tokens => #Cached(tokens))

module Fetch = {
  // Returns the known list of multiple accounts' tokens
  let tokenContracts = (config, ~accounts) => {
    open ServerAPI
    config
    ->URL.Explorer.Tzkt.tokenRegistryUrl(~accounts)
    ->URL.get
    ->Promise.mapOk(tokens => {
      tokens
      ->Js.Json.decodeArray
      ->Option.getExn
      ->Array.map(obj => obj->JsonEx.decode(TokenContract.Decode.tzktDecoder)->Result.getExn)
      ->Array.reduce(PublicKeyHash.Map.empty, (contracts, t) =>
        contracts->PublicKeyHash.Map.set(t.token.address, t.token)
      )
    })
  }

  // If the result from BetterCallDev does not contain enough information, fetch
  // the metadata from the node using Taquito's API
  let fetchIfNecessary = (network: Network.t, tokenContract, tzktToken: Tzkt.t, tzip12Cache) => {
    let fetchMetadata = () =>
      tzip12Cache
      ->TaquitoAPI.Tzip12Cache.findContract(tzktToken->Tzkt.address)
      ->Promise.flatMapOk(contract => MetadataAPI.Tzip12.read(contract, tzktToken->Tzkt.tokenId))
      ->Promise.mapOk(metadata =>
        metadataToToken(network.chain->Network.getChainId, tokenContract, metadata)
      )
    switch Tzkt.toTokenRepr(network.chain->Network.getChainId->Some, tzktToken) {
    | Some(token) => Promise.ok(token)
    | None => fetchMetadata()
    }
  }

  let updatePartial = (error, chain, tzktToken: Tzkt.t) => {
    let retry = switch error {
    | MetadataAPI.TokenIdNotFound(_, _)
    | MetadataAPI.NoTzip12Metadata(_) => false
    | _ => true
    }
    let chainId = chain->Network.getChainId->Some
    let token =
      Tzkt.toTokenRepr(chainId, tzktToken)->Option.mapWithDefault(
        TokensLibrary.Token.Partial(tzktToken->Tzkt.updateFromBuiltinTemplate, chain, retry),
        t => Full(t),
      )
    (token, tzktToken.balance)
  }

  let getTokenRepr = (network: Network.t, tokenContract, tzktToken: Tzkt.t, tzip12Cache, inCache) =>
    switch inCache {
    | None
    | Some(TokensLibrary.Token.Partial(_, _, true)) =>
      fetchIfNecessary(network, tokenContract, tzktToken, tzip12Cache)->Promise.map(res =>
        switch res {
        | Error(e) => updatePartial(e, network.chain, tzktToken)
        | Ok(t) => (TokensLibrary.Token.Full(t), tzktToken.balance)
        }
      )

    | Some(t) => Promise.value((t, tzktToken.balance))
    }

  let addContractIfNecessary = (cache: TokensLibrary.t, tzip12Cache, address) => {
    let contract = cache->PublicKeyHash.Map.get(address)

    let fetchName = () =>
      tzip12Cache
      ->TaquitoAPI.Tzip12Cache.findContract(address)
      ->Promise.flatMapOk(contract => MetadataAPI.Tzip12.readContractMetadata(contract))
      ->Promise.mapOk(metadata => metadata.metadata.name)

    switch contract {
    | Some(_) => cache->Promise.value
    | None =>
      fetchName()->Promise.map(name => {
        let contract = {
          open TokensLibrary.Generic
          {
            name: Result.getWithDefault(name, None),
            address: address,
            tokens: Map.Int.empty,
          }
        }
        cache->PublicKeyHash.Map.set(address, contract)
      })
    }
  }

  let pruneMissingContracts = (bcdTokens, config, contracts, cache: TokensLibrary.t) => {
    // In an ideal world, the indexer always returns all the contracts
    let tryInTokenContracts = (bcdTokens, token, ids) =>
      contracts
      ->PublicKeyHash.Map.get(token)
      ->Option.map((contract: TokenContract.t) =>
        bcdTokens->PublicKeyHash.Map.set(token, (contract, ids))
      )

    // Check the contract has been cached once: it has at least one token
    let tryInCache = (bcdTokens, token, ids) =>
      cache
      ->PublicKeyHash.Map.get(token)
      ->Option.flatMap(c => c.TokensLibrary.Generic.tokens->Map.Int.minimum)
      ->Option.map(((_, token)) => {
        let address = TokensLibrary.Token.address(token)
        let kind = TokensLibrary.Token.kind(token)
        PublicKeyHash.Map.set(
          bcdTokens,
          address,
          (
            {
              open TokenContract
              {address: address, kind: kind}
            },
            ids,
          ),
        )
      })

    // Finally, we ask Indexer for the kind of the token (it might not have been
    // correctly indexed)
    let tryInAPI = (bcdTokens, token: PublicKeyHash.t, ids) =>
      NodeAPI.Tokens.checkTokenContract(config, token)->Promise.map(kind =>
        switch kind {
        | Ok(#...TokenContract.kind as kind) =>
          bcdTokens->PublicKeyHash.Map.set(
            token,
            (
              {
                open TokenContract
                {address: token, kind: kind}
              },
              ids,
            ),
          )
        | _ => bcdTokens
        }
      )

    bcdTokens->PublicKeyHash.Map.reduce(PublicKeyHash.Map.empty->Promise.value, (
      bcdTokens,
      token,
      ids,
    ) =>
      bcdTokens->Promise.flatMap(bcdTokens =>
        switch bcdTokens->tryInTokenContracts(token, ids) {
        | Some(bcdTokens) => bcdTokens->Promise.value
        | None =>
          bcdTokens
          ->tryInCache(token, ids)
          ->Option.mapWithDefault(bcdTokens->tryInAPI(token, ids), Promise.value)
        }
      )
    )
  }

  let updateCache = (cache, token) =>
    cache->TokensLibrary.Generic.updateToken(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
      ~updatedValue=x =>
        switch x {
        | None
        | Some(TokensLibrary.Token.Partial(_)) =>
          Some(token)
        | Some(t) => Some(t)
        },
    )

  let updateCacheWithBalance = (cache, (token, balance)) =>
    cache->TokensLibrary.Generic.updateToken(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
      ~updatedValue=x =>
        switch x {
        | None
        | Some((TokensLibrary.Token.Partial(_), _)) =>
          Some((token, balance))
        | Some((t, _)) => Some((t, balance))
        },
    )

  let handleUniqueToken = (
    network,
    tzip12Cache,
    tokenContract: TokenContract.t,
    ~onTokens=?,
    ~onStop=?,
    indexCacheTokens: FutureBase.t<(int, TokensLibrary.t, list<TokensLibrary.WithBalance.token>)>,
    _tokenId: int,
    token: Tzkt.t,
  ) =>
    if onStop->Option.mapWithDefault(false, f => f()) {
      indexCacheTokens
    } else {
      indexCacheTokens->Promise.flatMap(((index, cache, finalTokens)) => {
        let inCache =
          cache->TokensLibrary.Generic.getToken(token->Tzkt.address, token->Tzkt.tokenId)

        getTokenRepr(network, tokenContract, token, tzip12Cache, inCache)->Promise.map(((
          tokenRepr,
          balance,
        )) => {
          onTokens->Option.mapWithDefault((), f => f(~lastToken=index))

          (index + 1, cache->updateCache(tokenRepr), list{(tokenRepr, balance), ...finalTokens})
        })
      })
    }

  let handleTokens = (
    network,
    tzip12Cache,
    ~onTokens=?,
    ~onStop=?,
    indexCacheTokens: FutureBase.t<(int, TokensLibrary.t, list<TokensLibrary.WithBalance.token>)>,
    _,
    (tokenContract: TokenContract.t, tokens: Map.Int.t<Tzkt.t>),
  ) =>
    indexCacheTokens->Promise.flatMap(((index, cache, finalTokens)) =>
      cache
      ->addContractIfNecessary(tzip12Cache, tokenContract.address)
      ->Promise.flatMap(updatedCache =>
        tokens->Map.Int.reduce(
          (index, updatedCache, finalTokens)->FutureBase.value,
          handleUniqueToken(network, tzip12Cache, tokenContract, ~onTokens?, ~onStop?),
        )
      )
    )

  let fetchAccountsTokensRaw = (
    network,
    ~accounts,
    tzip12Cache,
    cache: TokensLibrary.t,
    ~onTokens=?,
    ~onStop=?,
    ~index,
    ~numberByAccount,
  ) =>
    TzktAPI.fetchAccountsTokens(network, accounts, index, numberByAccount)->Promise.flatMapOk(((
      tokens,
      nextIndex,
      _,
    )) => {
      let onTokens = onTokens->Option.map(f => f(~total=TzktAPI.tokensNumber(tokens)))

      let tokens =
        tokenContracts(network, ~accounts)->Promise.flatMapOk(tokensContracts =>
          tokens->pruneMissingContracts(network, tokensContracts, cache)->Promise.map(v => Ok(v))
        )

      tokens
      ->Promise.flatMapOk(tokens =>
        tokens
        ->PublicKeyHash.Map.reduce(
          (index, cache, list{})->Promise.value,
          handleTokens(network, tzip12Cache, ~onTokens?, ~onStop?),
        )
        ->Promise.map(v => Ok(v))
      )
      ->Promise.mapOk(((_, cache, tokensWithMetadata)) => (cache, tokensWithMetadata, nextIndex))
    })

  let updateContractNames = (tokens, cache) =>
    tokens->PublicKeyHash.Map.map((
      c: TokensLibrary.Generic.contract<TokensLibrary.WithBalance.token>,
    ) =>
      c.name == None
        ? {
            ...c,
            name: cache
            ->PublicKeyHash.Map.get(c.address)
            ->Option.mapWithDefault(None, c => c.TokensLibrary.Generic.name),
          }
        : c
    )

  let buildFromCache = (tokens, cache) =>
    tokens->List.reduce(PublicKeyHash.Map.empty, updateCacheWithBalance)->updateContractNames(cache)

  let fetchAccountsTokens = (
    network,
    ~accounts,
    ~index,
    ~numberByAccount,
    ~onTokens=?,
    ~onStop=?,
    (),
  ) =>
    TokenStorage.Cache.getWithFallback()
    ->Promise.value
    ->Promise.flatMapOk(cache => {
      let toolkit = MetadataAPI.toolkit(network)
      let tzip12Cache = TaquitoAPI.Tzip12Cache.make(toolkit)

      fetchAccountsTokensRaw(
        network,
        ~accounts,
        tzip12Cache,
        cache,
        ~index,
        ~numberByAccount,
        ~onTokens?,
        ~onStop?,
      )
    })
    ->Promise.mapOk(((updatedCache, tokens, nextIndex)) => {
      updatedCache->TokenStorage.Cache.set
      (updatedCache, tokens->buildFromCache(updatedCache), nextIndex)
    })

  let isRegistered = (registered, token) =>
    registered->RegisteredTokens.isRegistered(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
    )

  let partitionByRegistration = (tokens, registered) =>
    tokens->Array.partition(registered->isRegistered)

  let accountsTokens = (config, ~accounts, ~index, ~numberByAccount) =>
    fetchAccountsTokens(config, ~accounts, ~index, ~numberByAccount, ())->Promise.flatMapOk(((
      cache,
      _,
      nextIndex,
    )) =>
      TokenStorage.Registered.getWithFallback()
      ->Result.map(registered => {
        let (registered, toRegister) =
          cache->TokensLibrary.Generic.valuesToArray->partitionByRegistration(registered)

        (registered, toRegister, nextIndex)
      })
      ->Promise.value
    )

  let fetchAccountsTokensStreamed = (
    config,
    ~accounts,
    ~index,
    ~numberByAccount,
    ~onTokens,
    ~onStop,
    ~onceFinished,
  ) => {
    let rec get = (accumulatedTokens, index) =>
      fetchAccountsTokens(
        config,
        ~accounts,
        ~index,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        (),
      )->Promise.flatMapOk(((fullCache, fetchedTokens, nextIndex)) => {
        let accumulatedTokens =
          accumulatedTokens->TokensLibrary.WithBalance.mergeAndUpdateBalance(fetchedTokens)
        nextIndex <= index || onStop()
          ? onceFinished(fullCache, accumulatedTokens, nextIndex)
          : get(accumulatedTokens, nextIndex)
      })
    get(TokensLibrary.Generic.empty, index)
  }

  type fetched<'tokens> = [#Cached('tokens) | #Fetched('tokens, int)]

  type fetchedNFTs = fetched<TokensLibrary.WithBalance.t>

  let accountNFTs = (
    config,
    ~account,
    ~numberByAccount,
    ~onTokens,
    ~onStop,
    ~allowHidden,
    ~fromCache,
  ) => {
    let onceFinished = (_, tokens, number) =>
      tokens->registerNFTs(account)->Result.map(() => (tokens, number))->Promise.value

    let getFromCache = () =>
      registeredTokens(#NFT(account, allowHidden))->Result.map(tokens => #Cached(tokens))

    let getFromNetwork = () =>
      fetchAccountsTokensStreamed(
        config,
        ~accounts=list{account},
        ~index=0,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~onceFinished,
      )->Promise.mapOk(((tokens, number)) =>
        #Fetched(
          tokens->TokensLibrary.Generic.keepTokens((_, _, (token, _)) =>
            TokensLibrary.Token.isNFT(token)
          ),
          number,
        )
      )
    fromCache ? getFromCache()->Promise.value : getFromNetwork()
  }

  let accountsTokensNumber = (config, ~accounts) =>
    TzktAPI.fetchAccountsTokens(config, accounts, 0, 1)->Promise.mapOk(((_, _, total)) => total)

  type fetchedTokens = fetched<TokensLibrary.WithRegistration.t>

  let accountsFungibleTokensWithRegistration = (
    network,
    ~accounts,
    ~numberByAccount,
    ~onTokens,
    ~onStop,
    ~fromCache,
  ) => {
    let getFromCache = () => cachedTokensWithRegistration(network, #FT)

    let getFromNetwork = () => {
      let onceFinished = (fullCache, _, number) =>
        fullCache
        ->handleRegistrationStatus(token => token->keepToken(network, #FT) ? Some(token) : None)
        ->Promise.mapOk(tokens => (tokens, number))
      fetchAccountsTokensStreamed(
        network,
        ~accounts,
        ~index=0,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~onceFinished,
      )->Promise.mapOk(((tokens, number)) => #Fetched(tokens, number))
    }
    fromCache ? getFromCache() : getFromNetwork()
  }
}
