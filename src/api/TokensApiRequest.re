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

include ApiRequest;

let useCheckTokenContract = tokens => {
  let set = (~config: ConfigContext.env, address) =>
    switch (tokens->TokensLibrary.Generic.pickAnyAtAddress(address)) {
    | None => config.network->NodeAPI.Tokens.checkTokenContract(address)
    | Some((_, _, (token, _))) =>
      (
        token->TokensLibrary.Token.kind: TokenContract.kind :> [>
          TokenContract.kind
          | `NotAToken
        ]
      )
      ->Promise.ok
    };
  ApiRequest.useSetter(~set, ~kind=Logs.Tokens, ~toast=false, ());
};

let useLoadBalance =
    (
      ~requestState,
      ~address: PublicKeyHash.t,
      ~token: PublicKeyHash.t,
      ~kind: TokenRepr.kind,
    ) => {
  let get = (~config: ConfigContext.env, (address, token, kind)) =>
    switch (kind) {
    | TokenRepr.FA1_2 =>
      config.network->NodeAPI.Tokens.runFA12GetBalance(~address, ~token)
    | FA2(tokenId) =>
      config.network->NodeAPI.Tokens.callFA2BalanceOf(address, token, tokenId)
    };

  ApiRequest.useLoader(
    ~get,
    ~kind=Logs.Tokens,
    ~requestState,
    (address, token, kind),
  );
};

type nftCacheRequest = {
  holder: PublicKeyHash.t,
  allowHidden: bool,
};

type registry = {
  registered: array(TokensLibrary.Token.t),
  toRegister: array(TokensLibrary.Token.t),
  nextIndex: int,
};

type filter = [ | `Any | `FT | `NFT];

let useLoadTokensFromCache = requestState => {
  let get = (~config: ConfigContext.env, filter) => {
    TokensAPI.cachedTokensWithRegistration(config.network, filter);
  };
  ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState);
};

type withCache('request) = {
  fromCache: bool,
  request: 'request,
};

let useLoadTokensGeneric =
    (
      (apiRequest, setRequest),
      tokensNumberRequest,
      request,
      get,
      ~reloadOnForceExpired,
    ) => {
  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Tokens, ~setRequest, ());

  let isMounted = ReactUtils.useIsMounted();

  let conditionToLoad = (request, isMounted) => {
    let requestNotAskedAndMounted = request->isNotAsked && isMounted;
    let requestDoneButReloadOnMount = request->isDone && !isMounted;
    let requestExpired = request->isForceExpired && reloadOnForceExpired;
    requestNotAskedAndMounted || requestDoneButReloadOnMount || requestExpired;
  };

  React.useEffect5(
    () =>
      if (conditionToLoad(apiRequest, isMounted)) {
        getRequest(request)->ignore;
        None;
      } else {
        switch (apiRequest, tokensNumberRequest) {
        | (Done(Ok(`Cached(tokens)), _), Done(Ok(tokensNumber), _)) =>
          if (tokens->PublicKeyHash.Map.isEmpty
              && tokensNumber > 0
              && tokensNumber <= 50) {
            getRequest({...request, fromCache: false})->ignore;
          };
          None;
        | _ => None
        };
      },
    (isMounted, apiRequest, request, setRequest, tokensNumberRequest),
  );

  (apiRequest, getRequest);
};

module NFT = {
  type fetched = TokensAPI.Fetch.fetchedNFTs;

  type request = {
    account: PublicKeyHash.t,
    allowHidden: bool,
    numberByAccount: int,
  };

  let useFetchWithCache =
      (onTokens, onStop, request, tokensNumberRequest, nftRequest) => {
    let get =
        (
          ~config: ConfigContext.env,
          {fromCache, request: {account, allowHidden, numberByAccount}},
        ) => {
      TokensAPI.Fetch.accountNFTs(
        config.network,
        ~account,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~allowHidden,
        ~fromCache,
      );
    };

    useLoadTokensGeneric(
      request,
      tokensNumberRequest,
      nftRequest,
      get,
      ~reloadOnForceExpired=false,
    );
  };

  let useAccountTokensNumber = (requestState, account) => {
    let get = (~config: ConfigContext.env, account) =>
      TokensAPI.Fetch.accountsTokensNumber(
        config.network,
        ~accounts=[account],
      );

    ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, account);
  };
};

module Fungible = {
  type fetched = TokensAPI.Fetch.fetchedTokens;

  type request = {
    accounts: list(PublicKeyHash.t),
    numberByAccount: int,
  };

  let useFetchWithCache =
      (onTokens, onStop, request, tokensNumberRequest, tokensRequest) => {
    let get =
        (
          ~config: ConfigContext.env,
          {fromCache, request: {accounts, numberByAccount}},
        ) => {
      TokensAPI.Fetch.accountsFungibleTokensWithRegistration(
        config.network,
        ~accounts,
        ~numberByAccount,
        ~onTokens,
        ~onStop,
        ~fromCache,
      );
    };

    useLoadTokensGeneric(
      request,
      tokensNumberRequest,
      tokensRequest,
      get,
      ~reloadOnForceExpired=true,
    );
  };

  let useAccountsTokensNumber = (requestState, accounts) => {
    let get = (~config: ConfigContext.env, accounts) =>
      TokensAPI.Fetch.accountsTokensNumber(config.network, ~accounts);

    ApiRequest.useLoader(~get, ~kind=Logs.Tokens, ~requestState, accounts);
  };
};

let useDelete = (~sideEffect=?, pruneCache) => {
  let set = (~config as _, token) =>
    TokensAPI.removeToken(token, ~pruneCache)->Promise.value;

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_deleted,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config: ConfigContext.env, token) =>
    TokensAPI.addFungibleToken(config.network, token);

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

let useCacheToken = (~sideEffect=?, ()) => {
  let set = (~config: ConfigContext.env, token) =>
    TokensAPI.addTokenToCache(config.network, token);

  ApiRequest.useSetter(
    ~logOk=_ => I18n.token_created,
    ~toast=false,
    ~set,
    ~kind=Logs.Tokens,
    ~sideEffect?,
    (),
  );
};

type nfts = {
  tokens: TokensLibrary.WithBalance.t,
  holder: PublicKeyHash.t,
};
