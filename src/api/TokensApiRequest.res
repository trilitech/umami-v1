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

include ApiRequest

let useCheckTokenContract = tokens => {
  let set = (~config: ConfigContext.env, address) =>
    switch tokens->TokensLibrary.Generic.pickAnyAtAddress(address) {
    | None => config.network->NodeAPI.Tokens.checkTokenContract(address)
    | Some((_, _, (token, _))) =>
      (token->TokensLibrary.Token.kind: TokenContract.kind :> [
        | TokenContract.kind
        | #NotAToken
      ])->Promise.ok
    }
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ())
}

let getOneBalance = (balancesRequest, key) =>
  balancesRequest->ApiRequest.flatMap((balances, isDone, t) =>
    switch balances->TokenRepr.Map.get(key) {
    | Some(balance) if isDone => ApiRequest.Done(Ok(balance), t)
    | Some(balance) => Loading(Some(balance))
    | None if isDone => Done(Error(BalanceApiRequest.BalanceNotFound), t)
    | None => Loading(None)
    }
  )

let balancesfromArray = (
  array: array<(PublicKeyHash.t, TokenRepr.Unit.t)>,
  ~contract: PublicKeyHash.t,
  ~id: option<int>,
  ~tokenMap: ApiRequest.requestState<
    TokenRepr.Map.t<TokenRepr.Map.key, TokenRepr.Unit.t, TokenRepr.Map.id>,
  >,
) => {
  let (tokenMap, _) = tokenMap
  let balances = array->Array.map(((pkh, amount)) => ((pkh, (contract, id)), amount))
  let previousMap = switch tokenMap {
  | Done(Ok(map), _) => map
  | Loading(Some(map)) => map
  | _ => TokenRepr.Map.empty
  }
  balances->Array.reduce(previousMap, (map, (key, value)) => map->TokenRepr.Map.set(key, value))
}

let useLoadBalances = (
  ~forceFetch=true,
  ~requestState,
  ~addresses: list<PublicKeyHash.t>,
  ~selectedToken: option<(PublicKeyHash.t, TokenRepr.kind)>,
) => {
  let get = (~config: ConfigContext.env, (addresses, selectedToken)) =>
    switch selectedToken {
    | Some((token, kind)) =>
      let setup = (token, kind) =>
        config.network
        ->ServerAPI.Explorer.getTokenBalances(~addresses, ~contract=token, ~id=kind)
        ->Promise.mapOk(balancesfromArray(~contract=token, ~id=kind, ~tokenMap=requestState))
      switch kind {
      | TokenRepr.FA1_2 => setup(token, None)
      | FA2(tokenId) => setup(token, Some(tokenId))
      }
    | None => Promise.ok(TokenRepr.Map.empty)
    }

  ApiRequest.useLoader(
    ~get,
    ~condition=_ => !(addresses->Js.List.isEmpty) && (selectedToken != None && forceFetch),
    ~kind=Logs.Tokens,
    ~requestState,
    (addresses, selectedToken),
  )
}

type nftCacheRequest = {
  holder: PublicKeyHash.t,
  allowHidden: bool,
}

type registry = {
  registered: array<TokensLibrary.Token.t>,
  toRegister: array<TokensLibrary.Token.t>,
  nextIndex: int,
}

type filter = [#Any | #FT | #NFT]

let useLoadTokensFromCache = requestState => {
  let get = (~config: ConfigContext.env, filter) =>
    TokensAPI.cachedTokensWithRegistration(config.network, filter)
  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState)
}

type withCache<'request> = {
  fromCache: bool,
  request: 'request,
}

let useLoadTokensGeneric = (
  (apiRequest, setRequest),
  tokensNumberRequest,
  request,
  get,
  ~reloadOnForceExpired,
) => {
  let getRequest = ApiRequest.useGetter(~get, ~kind=Logs.Tokens, ~setRequest, ())

  let isMounted = ReactUtils.useIsMounted()

  let conditionToLoad = (request, isMounted) => {
    let requestNotAskedAndMounted = request->isNotAsked && isMounted
    let requestDoneButReloadOnMount = request->isDone && !isMounted
    let requestExpired = request->isForceExpired && reloadOnForceExpired
    requestNotAskedAndMounted || (requestDoneButReloadOnMount || requestExpired)
  }

  React.useEffect5(() =>
    if conditionToLoad(apiRequest, isMounted) {
      getRequest(request)->ignore
      None
    } else {
      switch (apiRequest, tokensNumberRequest) {
      | (Done(Ok(#Cached(tokens)), _), Done(Ok(tokensNumber), _)) =>
        if tokens->PublicKeyHash.Map.isEmpty && (tokensNumber > 0 && tokensNumber <= 50) {
          getRequest({...request, fromCache: false})->ignore
        }
        None
      | _ => None
      }
    }
  , (isMounted, apiRequest, request, setRequest, tokensNumberRequest))

  (apiRequest, getRequest)
}

module NFT = {
  type fetched = TokensAPI.Fetch.fetchedNFTs

  type request = {
    account: PublicKeyHash.t,
    allowHidden: bool,
    numberByAccount: int,
  }

  let useFetchWithCache = (onTokens, onStop, request, tokensNumberRequest, nftRequest) => {
    let get = (
      ~config: ConfigContext.env,
      {fromCache, request: {account, allowHidden, numberByAccount}},
    ) =>
      TokensAPI.Fetch.accountNFTs(
        config.network,
        ~account,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~allowHidden,
        ~fromCache,
      )

    useLoadTokensGeneric(request, tokensNumberRequest, nftRequest, get, ~reloadOnForceExpired=false)
  }

  let useAccountTokensNumber = (requestState, account) => {
    let get = (~config: ConfigContext.env, account) =>
      TokensAPI.Fetch.accountsTokensNumber(config.network, ~accounts=list{account})

    ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, account)
  }
}

module Fungible = {
  type fetched = TokensAPI.Fetch.fetchedTokens

  type request = {
    accounts: list<PublicKeyHash.t>,
    numberByAccount: int,
  }

  let useFetchWithCache = (onTokens, onStop, request, tokensNumberRequest, tokensRequest) => {
    let get = (~config: ConfigContext.env, {fromCache, request: {accounts, numberByAccount}}) =>
      TokensAPI.Fetch.accountsFungibleTokensWithRegistration(
        config.network,
        ~accounts,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~fromCache,
      )

    useLoadTokensGeneric(
      request,
      tokensNumberRequest,
      tokensRequest,
      get,
      ~reloadOnForceExpired=true,
    )
  }

  let useAccountsTokensNumber = (requestState, accounts) => {
    let get = (~config: ConfigContext.env, accounts) =>
      TokensAPI.Fetch.accountsTokensNumber(config.network, ~accounts)

    ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, accounts)
  }
}

let useDelete = (~sideEffect=?, pruneCache) => {
  let set = (~config as _, token) => TokensAPI.removeToken(token, ~pruneCache)->Promise.value

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_deleted,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  )
}

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config: ConfigContext.env, token) => TokensAPI.addFungibleToken(config.network, token)

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  )
}

let useCacheToken = (~sideEffect=?, ()) => {
  let set = (~config: ConfigContext.env, token) => TokensAPI.addTokenToCache(config.network, token)

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  )
}

type nfts = {
  tokens: TokensLibrary.WithBalance.t,
  holder: PublicKeyHash.t,
}
