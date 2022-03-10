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

/* open UmamiCommon; */
open Let;

type Errors.t +=
  | NotFAContract(string)
  | RegisterNotAFungibleToken(PublicKeyHash.t, TokenRepr.kind)
  | RegisterNotANonFungibleToken(PublicKeyHash.t, TokenRepr.kind);

let () =
  Errors.registerHandler(
    "Tokens",
    fun
    | NotFAContract(_) => I18n.error_check_contract->Some
    | RegisterNotAFungibleToken(_) => I18n.error_register_not_fungible->Some
    | RegisterNotANonFungibleToken(_) =>
      I18n.error_register_not_non_fungible->Some
    | _ => None,
  );

module BetterCallDev = {
  open BCD;

  let mergeBalanceArray = (fetched, balances) => {
    let add = (map, bcdToken) => {
      let f = tokens =>
        tokens
        ->Option.getWithDefault(Map.Int.empty)
        ->Map.Int.set(bcdToken.token_id, bcdToken)
        ->Some;
      map->PublicKeyHash.Map.update(bcdToken.contract, f);
    };

    balances->Array.reduce(fetched, add);
  };

  let tokensNumber = tokens => {
    tokens->PublicKeyHash.Map.reduce(0, (total, _, ids) =>
      ids->Map.Int.size + total
    );
  };

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let fetchTokens = (config, alreadyFetched, account, index, number, total) => {
    let rec fetch = (alreadyFetched, index, number) => {
      let%Await url =
        ServerAPI.URL.External.betterCallDevAccountTokens(
          ~config,
          ~account,
          ~sortBy=`TokenId,
          ~limit=min(requestPageSize, number),
          ~index,
          ~hideEmpty=true,
          (),
        )
        ->Promise.value;
      let%Await json = url->ServerAPI.URL.get;
      let%Await tokens = json->JsonEx.decode(Decode.decoder)->Promise.value;

      // corresponds to the number of tokens to fetch according to
      // the number we ask, not the total remaining
      let remaining = max(number - requestPageSize, 0);

      // index for the next fetch
      let index = index + tokens.balances->Array.size;

      // Final index
      let finalIndex = min(index, tokens.total);

      let remainingOnTotal = max(total - index, 0);

      // adds the newly fetched tokens to the cache currently built
      let alreadyFetched =
        alreadyFetched->(mergeBalanceArray(tokens.balances));

      remaining <= 0 || remainingOnTotal <= 0
        ? (alreadyFetched, finalIndex)->Promise.ok
        : fetch(alreadyFetched, index, remaining);
    };
    fetch(alreadyFetched, index, number);
  };

  let fetchTokensNumber = (config, account) => {
    let%Await url =
      ServerAPI.URL.External.tzktAccountTokensNumber(~config, ~account)
      ->FutureBase.value;
    let%Await number = ServerAPI.URL.get(url);
    number->JsonEx.decode(Json.Decode.int)->Promise.value;
  };

  // fetch a certain number of token for each account
  let fetchAccountsTokens = (config, accounts, index, numberByAccount) => {
    let rec fetch = (alreadyFetched, accounts, highestIndex, totalAccumulated) => {
      switch (accounts) {
      // the highest index corresponds to the accounts that has the most of
      // tokens. This is useful to know if this is the last page to fetch
      // (highestIndex < index) or if there are more tokens on at least one
      // account
      | [] => (alreadyFetched, highestIndex, totalAccumulated)->Promise.ok
      | [account, ...accounts] =>
        let%Await total = fetchTokensNumber(config, account);
        let%Await (fetched, index) =
          fetchTokens(
            config,
            alreadyFetched,
            account,
            index,
            numberByAccount,
            total,
          );
        fetch(
          fetched,
          accounts,
          max(index, highestIndex),
          total + totalAccumulated,
        );
      };
    };

    fetch(PublicKeyHash.Map.empty, accounts, index, 0);
  };

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let _fetchTokensBatchAccounts = (config, accounts) => {
    // The request accepts 10 accounts at most
    let rec fetch = (alreadyFetched, offset) =>
      if (offset >= accounts->Array.length) {
        alreadyFetched->Promise.ok;
      } else {
        let%Await url =
          ServerAPI.URL.External.betterCallDevBatchAccounts(
            ~config,
            ~accounts=accounts->Array.slice(~offset, ~len=10),
          )
          ->Promise.value;
        let%Await json = url->ServerAPI.URL.get;
        let%Await tokens = json->JsonEx.decode(Decode.decoder)->Promise.value;
        fetch([tokens.balances, ...alreadyFetched], offset + 10);
      };
    fetch([], 0);
  };
};

type filter = [ | `Any | `FT | `NFT(PublicKeyHash.t, bool)];

/* From a list of tokens and the cache, reconstructs the list of tokens with
   their metadata the user has registered */
let unfoldRegistered =
    (tokens, cache: TokensLibrary.t, filter): TokensLibrary.WithBalance.t => {
  let mergeTokens = (_, registeredToken, cachedToken) => {
    switch (registeredToken, cachedToken) {
    | (None, _)
    | (Some(_), None) => None
    | (Some(registered), Some(cached)) =>
      switch (registered, filter) {
      | (RegisteredTokens.NFT(info), `Any) => Some((cached, info.balance))
      | (FT, `FT | `Any) => Some((cached, ReBigNumber.fromInt(0)))
      | (NFT(info), `NFT(holder, allowHidden)) =>
        (!allowHidden ? !info.hidden : true) && info.holder == holder
          ? Some((cached, info.balance)) : None
      | _ => None
      }
    };
  };

  let merge = (_, registered, tokens) => {
    switch (registered, tokens) {
    | (None, _)
    // What should we do if the token is not in cache?
    | (_, None) => None
    | (Some(registeredContract), Some(cachedContract)) =>
      let tokens =
        Map.Int.merge(
          registeredContract.RegisteredTokens.tokens,
          cachedContract.TokensLibrary.Generic.tokens,
          mergeTokens,
        );

      tokens->Map.Int.isEmpty ? None : Some({...cachedContract, tokens});
    };
  };
  PublicKeyHash.Map.merge(tokens, cache, merge);
};

let registeredTokens = filter => {
  let%Res tokens = TokenStorage.Registered.getWithFallback();
  let%ResMap cache = TokenStorage.Cache.getWithFallback();
  tokens->unfoldRegistered(cache, filter);
};

let hiddenTokens = () => {
  let%ResMap tokens = TokenStorage.Registered.getWithFallback();
  tokens->RegisteredTokens.keepTokens((_, _) =>
    fun
    | FT => false
    | NFT({hidden}) => hidden
  );
};

// used for registration of custom tokens
let addTokenToCache = (config, token) => {
  let address = TokensLibrary.Token.address(token);
  let%Await tokenKind = config->NodeAPI.Tokens.checkTokenContract(address);

  let%AwaitMap () =
    switch (tokenKind) {
    | #TokenContract.kind => Promise.ok()
    | _ => Promise.err(NotFAContract((address :> string)))
    };

  let tokens =
    TokenStorage.Cache.getWithFallback()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->TokensLibrary.addToken(token);

  TokenStorage.Cache.set(tokens);
};

let addTokenToRegistered = (token, kind) => {
  let tokens =
    TokenStorage.Registered.getWithFallback()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->RegisteredTokens.registerToken(token, kind);

  TokenStorage.Registered.set(tokens);
};

let registerNFTs = (tokens, holder) => {
  let%ResMap registered = TokenStorage.Registered.getWithFallback();
  registered
  ->TokenStorage.mergeAccountNFTs(tokens, holder)
  ->TokenStorage.Registered.set;
};

let updateNFTsVisibility = (updatedTokens, ~hidden) => {
  let%ResMap registered = TokenStorage.Registered.getWithFallback();
  let registered =
    registered->RegisteredTokens.updateNFTsVisibility(updatedTokens, hidden);
  registered->TokenStorage.Registered.set;
  registered;
};

let addTokenToCache = (config, token) =>
  addTokenToCache(config, Full(token));

let addFungibleToken = (config, token) => {
  let%Await () =
    token->TokenRepr.isNFT
      ? Promise.err(RegisterNotAFungibleToken(token.address, token.kind))
      : Promise.ok();
  let%AwaitMap () = addTokenToCache(config, token);
  addTokenToRegistered(token, RegisteredTokens.FT);
};

let addNonFungibleToken = (config, token, holder, balance) => {
  let%Await () =
    token->TokenRepr.isNFT
      ? Promise.ok()
      : Promise.err(RegisterNotANonFungibleToken(token.address, token.kind));
  let%AwaitMap () = addTokenToCache(config, token);
  addTokenToRegistered(
    token,
    RegisteredTokens.(NFT({holder, hidden: false, balance})),
  );
};

let removeFromCache = token => {
  let%ResMap tokens = TokenStorage.Cache.getWithFallback();
  tokens->TokensLibrary.removeToken(token)->TokenStorage.Cache.set;
};

let removeFromRegistered = (token: Token.t) => {
  let%ResMap tokens = TokenStorage.Registered.getWithFallback();
  TokenStorage.Registered.set(
    tokens->RegisteredTokens.removeToken(token.address, TokenRepr.id(token)),
  );
};

let removeToken = (token, ~pruneCache) => {
  let%Res () = pruneCache ? removeFromCache(Full(token)) : Ok();
  removeFromRegistered(token);
};

let metadataToAsset = (metadata: ReTaquitoTypes.Tzip12.metadata) =>
  TokenRepr.Metadata.{
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
    thumbnailUri:
      TokenRepr.thumbnailUriFromFormat(
        metadata.thumbnailUri,
        metadata.formats,
      ),
    isTransferable: metadata.isTransferable->Option.getWithDefault(true),
    isBooleanAmount: metadata.isBooleanAmount->Option.getWithDefault(false),
    shouldPreferSymbol:
      metadata.shouldPreferSymbol->Option.getWithDefault(false),
    formats: metadata.formats,
    attributes: metadata.attributes,
  };

let metadataToToken =
    (
      chain,
      tokenContract: TokenContract.t,
      metadata: ReTaquitoTypes.Tzip12.metadata,
    ) =>
  TokenRepr.{
    kind: TokenContract.toTokenKind(tokenContract.kind, metadata.token_id),
    address: tokenContract.address,
    alias: metadata.name,
    symbol: metadata.symbol,
    decimals: metadata.decimals,
    chain,
    asset: metadata->metadataToAsset,
  };

let handleRegistrationStatus = (cache, keepMap) => {
  let%AwaitMap registered =
    TokenStorage.Registered.getWithFallback()->Promise.value;

  TokensLibrary.WithRegistration.keepAndSetRegistration(
    cache,
    registered,
    keepMap,
  );
};

let keepToken = (token, config: ConfigContext.env, filter) => {
  let kindOk =
    switch (filter) {
    | `Any => true
    | `FT => !token->TokensLibrary.Token.isNFT
    | `NFT => token->TokensLibrary.Token.isNFT
    };

  TokensLibrary.Token.chain(token)
  == config.network.Network.chain->Network.getChainId->Some
  && kindOk;
};

let cachedTokensWithRegistration = (config, filter) => {
  let%Await cache = TokenStorage.Cache.getWithFallback()->Promise.value;
  let%AwaitMap tokens =
    cache->handleRegistrationStatus(token =>
      token->keepToken(config, filter) ? Some(token) : None
    );
  `Cached(tokens);
};

module Fetch = {
  // Returns the known list of multiple accounts' tokens
  let tokenContracts = (config, ~accounts, ~kinds=?, ~limit=?, ~index=?, ()) => {
    open ServerAPI;

    let%Await tokens =
      URL.Explorer.tokenRegistry(
        config,
        ~accountsFilter=accounts,
        ~kinds?,
        ~limit?,
        ~index?,
        (),
      )
      ->URL.get;

    tokens->JsonEx.decode(TokenContract.Decode.map)->Promise.value;
  };

  let tokenRegistry = (config, ~kinds, ~limit, ~index, ()) =>
    tokenContracts(config, ~accounts=[], ~kinds, ~limit, ~index, ());

  // If the result from BetterCallDev does not contain enough information, fetch
  // the metadata from the node using Taquito's API
  let fetchIfNecessary =
      (
        config: ConfigContext.env,
        tokenContract,
        bcdToken: BCD.tokenBalance,
        tzip12Cache,
      ) => {
    let fetchMetadata = () => {
      let%Await contract =
        tzip12Cache->TaquitoAPI.Tzip12Cache.findContract(bcdToken.contract);
      let%AwaitMap metadata =
        MetadataAPI.Tzip12.read(contract, bcdToken.token_id);
      metadataToToken(
        config.network.chain->Network.getChainId,
        tokenContract,
        metadata,
      );
    };
    switch (BCD.toTokenRepr(tokenContract, bcdToken)) {
    | Some(token) => Promise.ok(token)
    | None => fetchMetadata()
    };
  };

  let updatePartial = (error, tokenContract, bcdToken: BCD.tokenBalance) => {
    let retry =
      switch (error) {
      | MetadataAPI.TokenIdNotFound(_, _)
      | MetadataAPI.NoTzip12Metadata(_) => false
      | _ => true
      };
    let token =
      BCD.toTokenRepr(tokenContract, bcdToken)
      ->Option.mapWithDefault(
          TokensLibrary.Token.Partial(
            tokenContract,
            bcdToken->BCD.updateFromBuiltinTemplate,
            retry,
          ),
          t =>
          Full(t)
        );
    (token, bcdToken.balance);
  };

  let getTokenRepr =
      (
        config,
        tokenContract,
        bcdToken: BCD.tokenBalance,
        tzip12Cache,
        inCache,
      ) => {
    switch (inCache) {
    | None
    | Some(TokensLibrary.Token.Partial(_, _, true)) =>
      let%FtMap res =
        fetchIfNecessary(config, tokenContract, bcdToken, tzip12Cache);
      switch (res) {
      | Error(e) => updatePartial(e, tokenContract, bcdToken)
      | Ok(t) => (TokensLibrary.Token.Full(t), bcdToken.balance)
      };

    | Some(t) => Promise.value((t, bcdToken.balance))
    };
  };

  let addContractIfNecessary = (cache: TokensLibrary.t, tzip12Cache, address) => {
    let contract = cache->PublicKeyHash.Map.get(address);

    let fetchName = () => {
      let%Await contract =
        tzip12Cache->TaquitoAPI.Tzip12Cache.findContract(address);
      let%AwaitMap metadata =
        MetadataAPI.Tzip12.readContractMetadata(contract);
      metadata.metadata.name;
    };

    switch (contract) {
    | Some(_) => cache->Promise.value
    | None =>
      let%FtMap name = fetchName();
      let contract =
        TokensLibrary.Generic.{
          name: Result.getWithDefault(name, None),
          address,
          tokens: Map.Int.empty,
        };
      cache->PublicKeyHash.Map.set(address, contract);
    };
  };

  let pruneMissingContracts =
      (bcdTokens, config, contracts, cache: TokensLibrary.t) => {
    // In an ideal world, the indexer always returns all the contracts
    let tryInTokenContracts = (bcdTokens, token, ids) =>
      contracts
      ->PublicKeyHash.Map.get(token)
      ->Option.map((contract: TokenContract.t) =>
          bcdTokens->PublicKeyHash.Map.set(token, (contract, ids))
        );

    // Check the contract has been cached once: it has at least one token
    let tryInCache = (bcdTokens, token, ids) =>
      cache
      ->PublicKeyHash.Map.get(token)
      ->Option.flatMap(c => c.TokensLibrary.Generic.tokens->Map.Int.minimum)
      ->Option.map(((_, token)) => {
          let address = TokensLibrary.Token.address(token);
          let kind = TokensLibrary.Token.kind(token);
          PublicKeyHash.Map.set(
            bcdTokens,
            address,
            (TokenContract.{address, kind}, ids),
          );
        });

    // Finally, we ask Mezos for the kind of the token (it might not have been
    // correctly indexed)
    let tryInAPI = (bcdTokens, token: PublicKeyHash.t, ids) => {
      let%FtMap kind = NodeAPI.Tokens.checkTokenContract(config, token);
      switch (kind) {
      | Ok(#TokenContract.kind as kind) =>
        bcdTokens->PublicKeyHash.Map.set(
          token,
          (TokenContract.{address: token, kind}, ids),
        )
      | _ => bcdTokens
      };
    };

    bcdTokens->PublicKeyHash.Map.reduce(
      PublicKeyHash.Map.empty->Promise.value,
      (bcdTokens, token, ids) => {
        let%Ft bcdTokens = bcdTokens;
        switch (bcdTokens->tryInTokenContracts(token, ids)) {
        | Some(bcdTokens) => bcdTokens->Promise.value
        | None =>
          bcdTokens
          ->tryInCache(token, ids)
          ->Option.mapWithDefault(
              bcdTokens->tryInAPI(token, ids),
              Promise.value,
            )
        };
      },
    );
  };

  let updateCache = (cache, token) =>
    cache->TokensLibrary.Generic.updateToken(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
      ~updatedValue=
        fun
        | None
        | Some(TokensLibrary.Token.Partial(_)) => Some(token)
        | Some(t) => Some(t),
    );

  let updateCacheWithBalance = (cache, (token, balance)) =>
    cache->TokensLibrary.Generic.updateToken(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
      ~updatedValue=
        fun
        | None
        | Some((TokensLibrary.Token.Partial(_), _)) => Some((token, balance))
        | Some((t, _)) => Some((t, balance)),
    );

  let handleUniqueToken =
      (
        config: ConfigContext.env,
        tzip12Cache,
        tokenContract: TokenContract.t,
        ~onTokens=?,
        ~onStop=?,
        indexCacheTokens:
          FutureBase.t(
            (int, TokensLibrary.t, list(TokensLibrary.WithBalance.token)),
          ),
        _tokenId: int,
        token: BCD.tokenBalance,
      ) =>
    if (onStop->Option.mapWithDefault(false, f => f())) {
      indexCacheTokens;
    } else {
      let%Ft (index, cache, finalTokens) = indexCacheTokens;
      let inCache =
        cache->TokensLibrary.Generic.getToken(token.contract, token.token_id);

      let%FtMap (tokenRepr, balance) =
        getTokenRepr(config, tokenContract, token, tzip12Cache, inCache);

      onTokens->Option.mapWithDefault((), f => f(~lastToken=index));

      (
        index + 1,
        cache->updateCache(tokenRepr),
        [(tokenRepr, balance), ...finalTokens],
      );
    };

  let handleTokens =
      (
        config: ConfigContext.env,
        tzip12Cache,
        ~onTokens=?,
        ~onStop=?,
        indexCacheTokens:
          FutureBase.t(
            (int, TokensLibrary.t, list(TokensLibrary.WithBalance.token)),
          ),
        _,
        (
          tokenContract: TokenContract.t,
          tokens: Map.Int.t(BCD.tokenBalance),
        ),
      ) => {
    let%Ft (index, cache, finalTokens) = indexCacheTokens;

    let%Ft updatedCache =
      cache->addContractIfNecessary(tzip12Cache, tokenContract.address);

    tokens->Map.Int.reduce(
      (index, updatedCache, finalTokens)->FutureBase.value,
      handleUniqueToken(
        config,
        tzip12Cache,
        tokenContract,
        ~onTokens?,
        ~onStop?,
      ),
    );
  };

  let fetchAccountsTokensRaw =
      (
        config: ConfigContext.env,
        ~accounts,
        tzip12Cache,
        cache: TokensLibrary.t,
        ~onTokens=?,
        ~onStop=?,
        ~index,
        ~numberByAccount,
      ) => {
    let%Await (tokens, nextIndex, _) =
      BetterCallDev.fetchAccountsTokens(
        config,
        accounts,
        index,
        numberByAccount,
      );

    let onTokens =
      onTokens->Option.map(f =>
        f(~total=BetterCallDev.tokensNumber(tokens))
      );

    let%Await tokenContracts = tokenContracts(config, ~accounts, ());

    let%Ft tokens =
      tokens->pruneMissingContracts(config, tokenContracts, cache);

    let%Ft (_, cache, tokensWithMetadata) =
      tokens->PublicKeyHash.Map.reduce(
        (index, cache, [])->Promise.value,
        handleTokens(config, tzip12Cache, ~onTokens?, ~onStop?),
      );

    (cache, tokensWithMetadata, nextIndex)->Promise.ok;
  };

  let updateContractNames = (tokens, cache) =>
    tokens->PublicKeyHash.Map.map(
      (c: TokensLibrary.Generic.contract(TokensLibrary.WithBalance.token)) =>
      c.name == None
        ? {
          ...c,
          name:
            cache
            ->PublicKeyHash.Map.get(c.address)
            ->Option.mapWithDefault(None, c => c.TokensLibrary.Generic.name),
        }
        : c
    );

  let buildFromCache = (tokens, cache) =>
    tokens
    ->List.reduce(PublicKeyHash.Map.empty, updateCacheWithBalance)
    ->updateContractNames(cache);

  let fetchAccountsTokens =
      (
        config: ConfigContext.env,
        ~accounts,
        ~index,
        ~numberByAccount,
        ~onTokens=?,
        ~onStop=?,
        (),
      ) => {
    let%Await cache = TokenStorage.Cache.getWithFallback()->Promise.value;

    let toolkit = MetadataAPI.toolkit(config);
    let tzip12Cache = TaquitoAPI.Tzip12Cache.make(toolkit);

    let%AwaitMap (updatedCache, tokens, nextIndex) =
      fetchAccountsTokensRaw(
        config,
        ~accounts,
        tzip12Cache,
        cache,
        ~index,
        ~numberByAccount,
        ~onTokens?,
        ~onStop?,
      );

    updatedCache->TokenStorage.Cache.set;
    (updatedCache, tokens->buildFromCache(updatedCache), nextIndex);
  };

  let isRegistered = (registered, token) => {
    registered->RegisteredTokens.isRegistered(
      TokensLibrary.Token.address(token),
      TokensLibrary.Token.id(token),
    );
  };

  let partitionByRegistration = (tokens, registered) => {
    tokens->Array.partition(registered->isRegistered);
  };

  let accountsTokens = (config, ~accounts, ~index, ~numberByAccount) => {
    let%AwaitRes (cache, _, nextIndex) =
      fetchAccountsTokens(config, ~accounts, ~index, ~numberByAccount, ());
    let%ResMap registered = TokenStorage.Registered.getWithFallback();
    let (registered, toRegister) =
      cache
      ->TokensLibrary.Generic.valuesToArray
      ->partitionByRegistration(registered);
    (registered, toRegister, nextIndex);
  };

  let fetchAccountsTokensStreamed =
      (
        config,
        ~accounts,
        ~index,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~onceFinished,
      ) => {
    let rec get = (accumulatedTokens, index) => {
      let%Await (fullCache, fetchedTokens, nextIndex) =
        fetchAccountsTokens(
          config,
          ~accounts,
          ~index,
          ~numberByAccount,
          ~onTokens,
          ~onStop,
          (),
        );
      let accumulatedTokens =
        accumulatedTokens->TokensLibrary.WithBalance.mergeAndUpdateBalance(
          fetchedTokens,
        );
      nextIndex <= index || onStop()
        ? onceFinished(fullCache, accumulatedTokens, nextIndex)
        : get(accumulatedTokens, nextIndex);
    };
    get(TokensLibrary.Generic.empty, index);
  };

  type fetched('tokens) = [ | `Cached('tokens) | `Fetched('tokens, int)];

  type fetchedNFTs = fetched(TokensLibrary.WithBalance.t);

  let accountNFTs =
      (
        config,
        ~account,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~allowHidden,
        ~fromCache,
      ) => {
    let onceFinished = (_, tokens, number) => {
      let%Await () = tokens->registerNFTs(account)->FutureBase.value;
      Promise.ok((tokens, number));
    };

    let getFromCache = () => {
      let%AwaitMap tokens =
        registeredTokens(`NFT((account, allowHidden)))->Promise.value;
      `Cached(tokens);
    };

    let getFromNetwork = () => {
      let%AwaitMap (tokens, number) =
        fetchAccountsTokensStreamed(
          config,
          ~accounts=[account],
          ~index=0,
          ~numberByAccount,
          ~onTokens,
          ~onStop,
          ~onceFinished,
        );
      `Fetched((
        tokens->TokensLibrary.Generic.keepTokens((_, _, (token, _)) =>
          TokensLibrary.Token.isNFT(token)
        ),
        number,
      ));
    };
    fromCache ? getFromCache() : getFromNetwork();
  };

  let accountsTokensNumber = (config, ~accounts) => {
    let%AwaitMap (_, _, total) =
      BetterCallDev.fetchAccountsTokens(config, accounts, 0, 1);

    total;
  };

  type fetchedTokens = fetched(TokensLibrary.WithRegistration.t);

  let accountsFungibleTokensWithRegistration =
      (
        config: ConfigContext.env,
        ~accounts,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~fromCache,
      ) => {
    let getFromCache = () => cachedTokensWithRegistration(config, `FT);

    let getFromNetwork = () => {
      let onceFinished = (fullCache, _, number) => {
        let%AwaitMap tokens =
          fullCache->handleRegistrationStatus(token =>
            token->keepToken(config, `FT) ? Some(token) : None
          );
        (tokens, number);
      };
      let%AwaitMap (tokens, number) =
        fetchAccountsTokensStreamed(
          config,
          ~accounts,
          ~index=0,
          ~numberByAccount,
          ~onTokens,
          ~onStop,
          ~onceFinished,
        );
      `Fetched((tokens, number));
    };
    fromCache ? getFromCache() : getFromNetwork();
  };
};
