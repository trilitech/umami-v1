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
open TokenRegistry;

type Errors.t +=
  | NotFAContract(string);

let () =
  Errors.registerHandler(
    "Tokens",
    fun
    | NotFAContract(_) => I18n.t#error_check_contract->Some
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

  // Returns a list of arrays: this will be treated later, so we return it as raw as possible for now
  let fetchTokens = (config, alreadyFetched, account, index, number) => {
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

      let remainingOnTotal = max(tokens.total - index, 0);

      // adds the newly fetched tokens to the cache currently built
      let alreadyFetched =
        alreadyFetched->(mergeBalanceArray(tokens.balances));

      remaining <= 0 || remainingOnTotal <= 0
        ? (alreadyFetched, finalIndex)->Promise.ok
        : fetch(alreadyFetched, index, remaining);
    };
    fetch(alreadyFetched, index, number);
  };

  // fetch a certain number of token for each account
  let fetchAccountsTokens = (config, accounts, index, numberByAccount) => {
    let rec fetch = (alreadyFetched, accounts, highestIndex) => {
      switch (accounts) {
      // the highest index corresponds to the accounts that has the most of
      // tokens. This is useful to know if this is the last page to fetch
      // (highestIndex < index) or if there are more tokens on at least one
      // account
      | [] => (alreadyFetched, highestIndex)->Promise.ok
      | [account, ...accounts] =>
        let%Await (fetched, index) =
          fetchTokens(
            config,
            alreadyFetched,
            account,
            index,
            numberByAccount,
          );
        fetch(fetched, accounts, max(index, highestIndex));
      };
    };

    fetch(PublicKeyHash.Map.empty, accounts, index);
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
let unfoldRegistered = (tokens, cache: Cache.t, filter) => {
  let keep = (registered, id, _) =>
    Registered.(
      registered.tokens
      ->Map.Int.get(id)
      ->Option.mapWithDefault(false, t =>
          switch (t, filter) {
          | (_, `Any) => true
          | (FT, `FT) => true
          | (NFT(info), `NFT(holder, allowHidden)) =>
            (!allowHidden ? !info.hidden : true) && info.holder == holder
          | _ => false
          }
        )
    );

  let merge = (_, registered, tokens) => {
    switch (registered, tokens) {
    | (None, _)
    // What should we do if the token is not in cache?
    | (_, None) => None
    | (Some(registered), Some(contract)) =>
      let tokens = contract.Cache.tokens->Map.Int.keep(keep(registered));

      Some({...contract, tokens});
    };
  };
  PublicKeyHash.Map.merge(tokens, cache, merge);
};

let registeredTokens = filter => {
  let%Res tokens = Registered.get();
  let%ResMap cache = Cache.get();
  tokens->unfoldRegistered(cache, filter);
};

// used for registration of custom tokens
let addTokenToCache = (config, token) => {
  let address = Cache.tokenAddress(token);
  let%Await tokenKind = config->NodeAPI.Tokens.checkTokenContract(address);

  let%AwaitMap () =
    switch (tokenKind) {
    | #TokenContract.kind => Promise.ok()
    | _ => Promise.err(NotFAContract((address :> string)))
    };

  let tokens =
    Cache.get()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->Cache.addToken(token);

  Cache.set(tokens);
};

let addTokenToRegistered = (token, kind) => {
  let tokens =
    Registered.get()
    ->Result.getWithDefault(PublicKeyHash.Map.empty)
    ->Registered.registerToken(token, kind);

  Registered.set(tokens);
};

let registerNFTs = (tokens, holder) => {
  open Registered;
  let%ResMap registered = get();
  registered->TokenRegistry.mergeAccountNFTs(tokens, holder)->set;
};

let addFungibleToken = (config, token) => {
  let%AwaitMap () = addTokenToCache(config, Full(token));
  addTokenToRegistered(token, Registered.FT);
};

let addNonFungibleToken = (config, token, holder) => {
  let%AwaitMap () = addTokenToCache(config, Full(token));
  addTokenToRegistered(token, Registered.(NFT({holder, hidden: false})));
};

let removeFromCache = token => {
  let%ResMap tokens = Cache.get();
  tokens->Cache.removeToken(token)->Cache.set;
};

let removeFromRegistered = (token: Token.t) => {
  let%ResMap tokens = Registered.get();
  Registered.set(
    tokens->Registered.removeToken(token.address, TokenRepr.id(token)),
  );
};

let removeToken = (token, ~pruneCache) => {
  let%Res () = pruneCache ? removeFromCache(Full(token)) : Ok();
  removeFromRegistered(token);
};

// Returns the known list of multiple accounts' tokens
let fetchTokenContracts =
    (config, ~accounts, ~kinds=?, ~limit=?, ~index=?, ()) => {
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
    thumbnailUri: metadata.thumbnailUri,
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

let getTokenRepr =
    (config, tokenContract, bcdToken: BCD.tokenBalance, tzip12Cache, inCache) => {
  switch (inCache) {
  | None =>
    let%FtMap res =
      fetchIfNecessary(config, tokenContract, bcdToken, tzip12Cache);
    res->Result.mapWithDefault(Cache.Partial(tokenContract, bcdToken), t =>
      Cache.Full(t)
    );
  | Some(t) => Promise.value(t)
  };
};

let addContractIfNecessary = (cache: Cache.t, tzip12Cache, address) => {
  let contract = cache->PublicKeyHash.Map.get(address);

  let fetchName = () => {
    let%Await contract =
      tzip12Cache->TaquitoAPI.Tzip12Cache.findContract(address);
    let%AwaitMap metadata = MetadataAPI.Tzip12.readContractMetadata(contract);
    metadata.metadata.name;
  };

  switch (contract) {
  | Some(_) => cache->Promise.value
  | None =>
    let%FtMap name = fetchName();
    let contract =
      Cache.{
        name: Result.getWithDefault(name, None),
        address,
        tokens: Map.Int.empty,
      };
    cache->PublicKeyHash.Map.set(address, contract);
  };
};

let pruneMissingContracts = (bcdTokens, config, contracts, cache: Cache.t) => {
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
    ->Option.flatMap(c => c.Cache.tokens->Map.Int.minimum)
    ->Option.map(((_, token)) => {
        let address = Cache.tokenAddress(token);
        let kind = Cache.tokenKind(token);
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
  cache->Cache.updateToken(
    Cache.tokenAddress(token),
    Cache.tokenId(token),
    ~updatedValue=
      fun
      | None
      | Some(Partial(_)) => Some(token)
      | Some(t) => Some(t),
  );

let handleUniqueToken =
    (
      config: ConfigContext.env,
      tzip12Cache,
      tokenContract: TokenContract.t,
      cacheAndTokens: FutureBase.t((Cache.t, list(Cache.token))),
      _tokenId: int,
      token: BCD.tokenBalance,
    ) => {
  let%Ft (cache, finalTokens) = cacheAndTokens;

  let inCache = cache->Cache.getToken(token.contract, token.token_id);

  let%FtMap tokenRepr =
    getTokenRepr(config, tokenContract, token, tzip12Cache, inCache);

  (cache->updateCache(tokenRepr), [tokenRepr, ...finalTokens]);
};

let handleTokens =
    (
      config: ConfigContext.env,
      tzip12Cache,
      cacheAndTokens: FutureBase.t((Cache.t, list(Cache.token))),
      _,
      (tokenContract: TokenContract.t, tokens: Map.Int.t(BCD.tokenBalance)),
    ) => {
  let%Ft (cache, finalTokens) = cacheAndTokens;

  let%Ft updatedCache =
    cache->addContractIfNecessary(tzip12Cache, tokenContract.address);

  tokens->Map.Int.reduce(
    (updatedCache, finalTokens)->FutureBase.value,
    handleUniqueToken(config, tzip12Cache, tokenContract),
  );
};

let fetchAccountsTokensRaw =
    (
      config: ConfigContext.env,
      ~accounts,
      tzip12Cache,
      cache: Cache.t,
      ~index,
      ~numberByAccount,
    ) => {
  let%Await (tokens, nextIndex) =
    BetterCallDev.fetchAccountsTokens(
      config,
      accounts,
      index,
      numberByAccount,
    );

  let%Await tokenContracts = fetchTokenContracts(config, ~accounts, ());

  let%Ft tokens =
    tokens->pruneMissingContracts(config, tokenContracts, cache);

  let%Ft (cache, tokensWithMetadata) =
    tokens->PublicKeyHash.Map.reduce(
      (cache, [])->Promise.value,
      handleTokens(config, tzip12Cache),
    );

  (cache, tokensWithMetadata, nextIndex)->Promise.ok;
};

let updateContractNames = (tokens, cache) =>
  tokens->PublicKeyHash.Map.map((c: Cache.contract) =>
    c.name == None
      ? {
        ...c,
        name:
          cache
          ->PublicKeyHash.Map.get(c.address)
          ->Option.mapWithDefault(None, c => c.Cache.name),
      }
      : c
  );

let buildFromCache = (tokens, cache) =>
  tokens
  ->List.reduce(PublicKeyHash.Map.empty, updateCache)
  ->updateContractNames(cache);

let fetchAccountsTokens =
    (
      config: ConfigContext.env,
      ~accounts,
      ~index,
      ~numberByAccount,
      ~withFullCache,
    ) => {
  let%Await cache = Cache.get()->Promise.value;

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
    );

  updatedCache->Cache.set;
  (
    withFullCache ? updatedCache : tokens->buildFromCache(updatedCache),
    nextIndex,
  );
};

let isRegistered = (registered, token) => {
  registered->Registered.isRegistered(
    Cache.tokenAddress(token),
    Cache.tokenId(token),
  );
};

let partitionByRegistration = (tokens, registered) => {
  tokens->Array.partition(registered->isRegistered);
};

let fetchAccountsTokensRegistry =
    (config, ~accounts, ~index, ~numberByAccount) => {
  let%AwaitRes (cache, nextIndex) =
    fetchAccountsTokens(
      config,
      ~accounts,
      ~index,
      ~numberByAccount,
      ~withFullCache=true,
    );
  let%ResMap registered = Registered.get();
  let (registered, toRegister) =
    cache->Cache.valuesToArray->partitionByRegistration(registered);
  (registered, toRegister, nextIndex);
};

let fetchTokenRegistry = (config, ~kinds, ~limit, ~index, ()) =>
  fetchTokenContracts(config, ~accounts=[], ~kinds, ~limit, ~index, ());
