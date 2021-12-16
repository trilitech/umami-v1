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

type reactState('state) = ('state, ('state => 'state) => unit);

type error = Errors.t;

type requestsState('requestResponse) =
  Map.String.t(ApiRequest.t('requestResponse));

type requestState('requestResponse) =
  reactState(ApiRequest.t('requestResponse));

type apiRequestsState('requestResponse) =
  reactState(requestsState('requestResponse));

type nextState('value) = (unit => option('value), unit => unit);

type state = {
  selectedAccountState: reactState(option(PublicKeyHash.t)),
  selectedTokenState: reactState(option((PublicKeyHash.t, int))),
  accountsRequestState: requestState(PublicKeyHash.Map.map(Account.t)),
  secretsRequestState: reactState(ApiRequest.t(array(Secret.derived))),
  balanceRequestsState: apiRequestsState(Tez.t),
  delegateRequestsState: apiRequestsState(option(PublicKeyHash.t)),
  delegateInfoRequestsState:
    apiRequestsState(option(NodeAPI.Delegate.delegationInfo)),
  operationsRequestsState:
    apiRequestsState(OperationApiRequest.operationsResponse),
  operationsConfirmations: reactState(Set.String.t),
  aliasesRequestState:
    reactState(ApiRequest.t(PublicKeyHash.Map.map(Alias.t))),
  bakersRequestState: reactState(ApiRequest.t(array(Delegate.t))),
  tokensRequestState:
    reactState(ApiRequest.t(TokensApiRequest.Fungible.fetched)),
  fungibleTokensRequestState:
    reactState(ApiRequest.t(TokensApiRequest.Fungible.fetched)),
  nftsRequestsState: apiRequestsState(TokensLibrary.t),
  tokensRegistryRequestState:
    reactState(ApiRequest.t(TokensApiRequest.registry)),
  accountsTokensRequestState: apiRequestsState(TokensApiRequest.NFT.fetched),
  accountsTokensNumberRequestState: apiRequestsState(int),
  tokensNumberRequestState: requestState(int),
  balanceTokenRequestsState: apiRequestsState(Token.Unit.t),
  apiVersionRequestState: reactState(option(Network.apiVersion)),
  eulaSignatureRequestState: reactState(bool),
  beaconPeersRequestState: requestState(array(ReBeacon.peerInfo)),
  beaconClient: reactState(option(ReBeacon.WalletClient.t)),
  beaconPermissionsRequestState:
    reactState(ApiRequest.t(array(ReBeacon.permissionInfo))),
  beaconNextRequestState: nextState(ReBeacon.Message.Request.t),
};

// Context and Provider

let initialApiRequestsState = (Map.String.empty, _ => ());
let initialNFTRequestsState = (Map.String.empty, _ => ());

let initialState = {
  selectedAccountState: (None, _ => ()),
  selectedTokenState: (None, _ => ()),
  accountsRequestState: (NotAsked, _ => ()),
  secretsRequestState: (NotAsked, _ => ()),
  balanceRequestsState: initialApiRequestsState,
  delegateRequestsState: initialApiRequestsState,
  delegateInfoRequestsState: initialApiRequestsState,
  operationsRequestsState: initialApiRequestsState,
  operationsConfirmations: (Set.String.empty, _ => ()),
  aliasesRequestState: (NotAsked, _ => ()),
  bakersRequestState: (NotAsked, _ => ()),
  tokensRequestState: (NotAsked, _ => ()),
  fungibleTokensRequestState: (NotAsked, _ => ()),
  nftsRequestsState: initialNFTRequestsState,
  tokensRegistryRequestState: (NotAsked, _ => ()),
  accountsTokensRequestState: initialApiRequestsState,
  accountsTokensNumberRequestState: initialApiRequestsState,
  tokensNumberRequestState: (NotAsked, _ => ()),
  balanceTokenRequestsState: initialApiRequestsState,
  apiVersionRequestState: (None, _ => ()),
  eulaSignatureRequestState: (false, _ => ()),
  beaconClient: (None, _ => ()),
  beaconPeersRequestState: (NotAsked, _ => ()),
  beaconPermissionsRequestState: (NotAsked, _ => ()),
  beaconNextRequestState: (() => None, () => ()),
};

let context = React.createContext(initialState);

module Provider = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

// Final Provider

[@react.component]
let make = (~children) => {
  let config = ConfigContext.useContent();
  let addToast = LogsContext.useToast();

  let network = React.useMemo1(() => {config.network}, [|config|]);
  let networkStatus = ConfigContext.useNetworkStatus();

  let selectedAccountState = React.useState(() => None);

  let (selectedAccount, setSelectedAccount) = selectedAccountState;

  let selectedTokenState = React.useState(() => None);

  let accountsRequestState = React.useState(() => ApiRequest.NotAsked);
  let (
    accountsRequest: ApiRequest.t(PublicKeyHash.Map.map(_)),
    _setAccountsRequest,
  ) = accountsRequestState;

  let balanceRequestsState = React.useState(() => Map.String.empty);
  let delegateRequestsState = React.useState(() => Map.String.empty);
  let delegateInfoRequestsState = React.useState(() => Map.String.empty);
  let operationsRequestsState = React.useState(() => Map.String.empty);
  let accountsTokensRequestState = React.useState(() => Map.String.empty);
  let accountsTokensNumberRequestState =
    React.useState(() => Map.String.empty);
  let tokensNumberRequestState = React.useState(() => ApiRequest.NotAsked);
  let balanceTokenRequestsState = React.useState(() => Map.String.empty);
  let operationsConfirmations = React.useState(() => Set.String.empty);

  let aliasesRequestState = React.useState(() => ApiRequest.NotAsked);
  let bakersRequestState = React.useState(() => ApiRequest.NotAsked);
  let tokensRequestState = React.useState(() => ApiRequest.NotAsked);
  let fungibleTokensRequestState = React.useState(() => ApiRequest.NotAsked);
  let nftsRequestsState = React.useState(() => Map.String.empty);
  let tokensRegistryRequestState = React.useState(() => ApiRequest.NotAsked);
  let secretsRequestState = React.useState(() => ApiRequest.NotAsked);

  let beaconClient =
    React.useState(() => Some(BeaconApiRequest.makeClient()));
  let beaconPeersRequestState = React.useState(() => ApiRequest.NotAsked);
  let beaconPermissionsRequestState =
    React.useState(() => ApiRequest.NotAsked);
  let beaconNextRequestState =
    BeaconApiRequest.useNextRequestState(
      beaconClient,
      beaconPeersRequestState,
    );

  let apiVersionRequestState = React.useState(() => None);
  let (_, setApiVersion) = apiVersionRequestState;

  let (_, setEulaSignature) as eulaSignatureRequestState =
    React.useState(() => false);

  let _: ApiRequest.t(_) = SecretApiRequest.useLoad(secretsRequestState);
  let _: ApiRequest.t(_) = AccountApiRequest.useLoad(accountsRequestState);
  let _: ApiRequest.t(_) = AliasApiRequest.useLoad(aliasesRequestState);
  let _: ApiRequest.t(_) =
    TokensApiRequest.useLoadTokensFromCache(tokensRequestState, `FT);

  React.useEffect0(() => {
    setEulaSignature(_ => Disclaimer.needSigning());
    None;
  });

  ReactUtils.useAsyncEffect1(
    () => {
      let%AwaitMap (v: Network.apiVersion, _) =
        Network.checkConfiguration(
          config.network.explorer,
          config.network.endpoint,
        );
      setApiVersion(_ => Some(v));
      if (!Network.checkInBound(v.api)) {
        addToast(
          Logs.error(~origin=Settings, Network.API(NotSupported(v.api))),
        );
      };
    },
    [|network|],
  );

  let reset = () => {
    let setAccounts = snd(accountsRequestState);
    setAccounts(ApiRequest.expireCache);
    let setSecrets = snd(secretsRequestState);
    setSecrets(ApiRequest.expireCache);
    let setBalances = snd(balanceRequestsState);
    setBalances(balances => balances->Map.String.map(ApiRequest.expireCache));
    let setBalancesToken = snd(balanceTokenRequestsState);
    setBalancesToken(balances =>
      balances->Map.String.map(ApiRequest.expireCache)
    );
  };

  React.useEffect1(
    () => {
      networkStatus.previous == Some(Offline)
      && networkStatus.current == Online
        ? reset() : ();

      None;
    },
    [|networkStatus|],
  );

  // Select a default account if no one selected
  React.useEffect2(
    () => {
      if (selectedAccount->Option.isNone) {
        accountsRequest
        ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
        ->PublicKeyHash.Map.valuesToArray
        ->Array.get(0)
        ->Option.iter((account: Account.t) =>
            setSelectedAccount(_ => Some(account.address))
          );
      };
      None;
    },
    (accountsRequest, selectedAccount),
  );

  <Provider
    value={
      selectedAccountState,
      selectedTokenState,
      accountsRequestState,
      secretsRequestState,
      balanceRequestsState,
      delegateRequestsState,
      delegateInfoRequestsState,
      operationsRequestsState,
      operationsConfirmations,
      aliasesRequestState,
      bakersRequestState,
      tokensRequestState,
      fungibleTokensRequestState,
      nftsRequestsState,
      tokensRegistryRequestState,
      accountsTokensRequestState,
      accountsTokensNumberRequestState,
      tokensNumberRequestState,
      balanceTokenRequestsState,
      apiVersionRequestState,
      eulaSignatureRequestState,
      beaconClient,
      beaconNextRequestState,
      beaconPeersRequestState,
      beaconPermissionsRequestState,
    }>
    children
  </Provider>;
};

// Hooks

let useStoreContext = () => React.useContext(context);

// Utils

let useRequestsState = (getRequestsState, key: option(string)) => {
  let store = useStoreContext();
  let (requests, setRequests) = store->getRequestsState;

  let request =
    React.useMemo2(
      () =>
        key->Option.mapWithDefault(ApiRequest.NotAsked, key =>
          requests
          ->Map.String.get(key)
          ->Option.getWithDefault(ApiRequest.NotAsked)
        ),
      (key, requests),
    );

  let setRequest =
    React.useCallback2(
      newRequestSetter =>
        key->Option.iter(key =>
          setRequests((request: requestsState(_)) =>
            request->Map.String.update(
              key, (oldRequest: option(ApiRequest.t(_))) =>
              Some(
                newRequestSetter(
                  oldRequest->Option.getWithDefault(NotAsked),
                ),
              )
            )
          )
        ),
      (key, setRequests),
    );

  (request, setRequest);
};

let useGetRequestStateFromMap = useRequestsState;

let resetRequests = requestsState =>
  requestsState->Map.String.map(ApiRequest.expireCache);

let reloadRequests = requestsState =>
  requestsState->Map.String.map(ApiRequest.updateToLoadingState);

//

let useApiVersion = () => {
  let store = useStoreContext();
  store.apiVersionRequestState->fst;
};

let useEulaSignature = () => {
  let store = useStoreContext();
  store.eulaSignatureRequestState->fst;
};

let setEulaSignature = () => {
  let store = useStoreContext();
  store.eulaSignatureRequestState->snd;
};

module Balance = {
  let useRequestState = useRequestsState(store => store.balanceRequestsState);

  let useLoad = (address: PublicKeyHash.t) => {
    let requestState = useRequestState(Some((address :> string)));

    BalanceApiRequest.useLoad(~requestState, ~address);
  };

  type Errors.t +=
    | EveryBalancesFail;

  let () =
    Errors.registerHandler(
      "Context",
      fun
      | EveryBalancesFail => I18n.Errors.every_balances_fail->Some
      | _ => None,
    );

  let handleBalances = (accountsSize, requests, reduce) => {
    let allErrors = () => requests->Array.every(ApiRequest.isError);
    let getAllDoneOk = () => requests->Array.keepMap(ApiRequest.getDoneOk);

    if (allErrors()) {
      ApiRequest.Done(Error(EveryBalancesFail), Expired);
    } else {
      let allDone = getAllDoneOk();
      if (allDone->Array.size == accountsSize) {
        let total = allDone->reduce;

        Done(Ok(total), ApiRequest.initCache());
      } else {
        Loading(None);
      };
    };
  };

  let useGetTotal = () => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let requests = balanceRequests->Map.String.valuesToArray;
    let accounts =
      accountsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty);

    handleBalances(accounts->PublicKeyHash.Map.size, requests, a =>
      a->Array.reduce(Tez.zero, (acc, balanceRequest) => {
        Tez.Infix.(acc + balanceRequest)
      })
    );
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setBalanceRequests) = store.balanceRequestsState;
    () => setBalanceRequests(resetRequests);
  };
};

module BalanceToken = {
  let useRequestState =
    useRequestsState(store => store.balanceTokenRequestsState);

  let getRequestKey =
      (address: PublicKeyHash.t, tokenAddress: PublicKeyHash.t, tokenKind) =>
    (address :> string)
    ++ (tokenAddress :> string)
    ++ tokenKind->TokenRepr.kindId->Int.toString;

  let useLoad =
      (address: PublicKeyHash.t, token: PublicKeyHash.t, kind: TokenRepr.kind) => {
    let requestState =
      useRequestState(address->getRequestKey(token, kind)->Some);

    TokensApiRequest.useLoadBalance(~requestState, ~address, ~token, ~kind);
  };

  let useGetTotal = (tokenAddress, tokenKind) => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceTokenRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty);

    let requests =
      accounts
      ->PublicKeyHash.Map.valuesToArray
      ->Array.keepMap(account => {
          balanceRequests->Map.String.get(
            getRequestKey(account.address, tokenAddress, tokenKind),
          )
        });

    Balance.handleBalances(accounts->PublicKeyHash.Map.size, requests, a =>
      a->Array.reduce(Token.Unit.zero, (acc, balanceRequest) => {
        Token.Unit.Infix.(acc + balanceRequest)
      })
    );
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setBalanceTokenRequests) = store.balanceTokenRequestsState;
    () => setBalanceTokenRequests(resetRequests);
  };
};

module Delegate = {
  let useRequestState = useRequestsState(store => store.delegateRequestsState);

  let useLoad = (address: PublicKeyHash.t) => {
    let requestState = useRequestState(Some((address :> string)));

    DelegateApiRequest.useLoad(~requestState, ~address);
  };

  let useGetAll = () => {
    let store = useStoreContext();
    let (delegateRequests, _) = store.delegateRequestsState;

    delegateRequests
    ->Map.String.map(request =>
        request->ApiRequest.getDoneOk->Option.flatMap(v => v)
      )
    ->Map.String.keep((_k, v) => v->Option.isSome)
    ->Map.String.map(Option.getExn);
  };

  let useGetAllRequests = () => {
    let store = useStoreContext();
    let (delegateRequests, _) = store.delegateRequestsState;

    delegateRequests;
  };
};

module DelegateInfo = {
  let useRequestState =
    useRequestsState(store => store.delegateInfoRequestsState);

  let useLoad = (address: PublicKeyHash.t) => {
    let requestState = useRequestState(Some((address :> string)));

    DelegateApiRequest.useLoadInfo(~requestState, ~address);
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setDelegateRequests) = store.delegateRequestsState;
    let (_, setDelegateInfoRequests) = store.delegateInfoRequestsState;
    () => {
      setDelegateRequests(resetRequests);
      setDelegateInfoRequests(resetRequests);
    };
  };
};

module Operations = {
  let useRequestState =
    useRequestsState(store => store.operationsRequestsState);

  let useLoad = (~limit=?, ~types=?, ~address: PublicKeyHash.t, ()) => {
    let requestState = useRequestState((address->Some :> option(string)));

    OperationApiRequest.useLoad(
      ~requestState,
      ~limit?,
      ~types?,
      ~address,
      (),
    );
  };

  let useResetNames = () => {
    let store = useStoreContext();

    let (_, setOperationsRequests) = store.operationsRequestsState;
    () => {
      setOperationsRequests(resetRequests);
    };
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let resetBalances = Balance.useResetAll();
    let resetBalanceTokens = BalanceToken.useResetAll();
    let resetDelegatesAndDelegatesInfo = DelegateInfo.useResetAll();
    let (_, setOperationsRequests) = store.operationsRequestsState;
    () => {
      setOperationsRequests(resetRequests);
      resetBalances();
      resetBalanceTokens();
      resetDelegatesAndDelegatesInfo();
    };
  };

  let useReloadAll = () => {
    let store = useStoreContext();
    let resetBalances = Balance.useResetAll();
    let resetBalanceTokens = BalanceToken.useResetAll();
    let resetDelegatesAndDelegatesInfo = DelegateInfo.useResetAll();
    let (_, setOperationsRequests) = store.operationsRequestsState;
    () => {
      setOperationsRequests(reloadRequests);
      resetBalances();
      resetBalanceTokens();
      resetDelegatesAndDelegatesInfo();
    };
  };

  let useCreate = () => {
    let resetOperations = useResetAll();
    let settings = ConfigContext.useContent();
    OperationApiRequest.useCreate(
      ~sideEffect=
        hash => {
          OperationApiRequest.waitForConfirmation(settings, hash)
          ->Promise.get(_ => resetOperations())
        },
      (),
    );
  };

  let useSimulate = () => {
    OperationApiRequest.useSimulate();
  };
};

module Bakers = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.bakersRequestState;
  };

  let useLoad = () => {
    let requestState = useRequestState();

    DelegateApiRequest.useLoadBakers(~requestState);
  };
};

module Tokens = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.tokensRequestState;
  };

  let useFungibleRequestState = () => {
    let store = useStoreContext();
    store.fungibleTokensRequestState;
  };

  let useGetTokens = (requestState, filter) => {
    TokensApiRequest.useLoadTokensFromCache(requestState, filter)
    ->ApiRequest.mapWithDefault(
        TokensLibrary.Contracts.empty,
        fun
        | `Cached(tokens) => tokens
        // technically impossible, since useLoadTokens only returns `Cached,
        // but this is due to the unification with the request
        | `Fetched(tokens, _) => tokens,
      );
  };

  let useGetAll = () => {
    let tokensRequestState = useRequestState();
    useGetTokens(tokensRequestState, `Any);
  };

  let useGetAllFungible = () => {
    let fungibleTokensRequestState = useFungibleRequestState();
    useGetTokens(fungibleTokensRequestState, `FT);
  };

  let useRegistryRequestState = () => {
    let store = useStoreContext();
    store.tokensRegistryRequestState;
  };

  let useRegistry = request => {
    let registryRequestState = useRegistryRequestState();
    TokensApiRequest.useLoadTokensRegistry(registryRequestState, request);
  };

  let useAccountTokensNumber = (account: PublicKeyHash.t) => {
    let useRequestsState =
      useGetRequestStateFromMap(store =>
        store.accountsTokensNumberRequestState
      );

    let accountsTokensRequestState =
      useRequestsState((Some(account) :> option(string)));

    TokensApiRequest.NFT.useAccountTokensNumber(
      accountsTokensRequestState,
      account,
    );
  };

  let useAccountNFTs = (onTokens, onStop, account: PublicKeyHash.t) => {
    let useRequestsState =
      useGetRequestStateFromMap(store => store.accountsTokensRequestState);

    let accountsTokensRequestState =
      useRequestsState((Some(account) :> option(string)));

    let tokensNumberRequest = useAccountTokensNumber(account);

    TokensApiRequest.NFT.useFetchWithCache(
      onTokens,
      onStop,
      accountsTokensRequestState,
      tokensNumberRequest,
    );
  };

  let useTokensNumberRequest = () => {
    let store = useStoreContext();
    store.tokensNumberRequestState;
  };

  let useTokensNumber = (accounts: list(PublicKeyHash.t)) => {
    let requestState = useTokensNumberRequest();

    TokensApiRequest.Fungible.useAccountsTokensNumber(requestState, accounts);
  };

  let useFetchTokens = (onTokens, onStop, accounts: list(PublicKeyHash.t)) => {
    let requestState = useRequestState();

    let tokensNumberRequest = useTokensNumber(accounts);

    TokensApiRequest.Fungible.useFetchWithCache(
      onTokens,
      onStop,
      requestState,
      tokensNumberRequest,
    );
  };

  let useResetAll = () => {
    let (_, setTokensRequest) = useRequestState();
    let (_, setFungibleTokensRequest) = useRequestState();
    () => {
      setTokensRequest(ApiRequest.expireCache);
      setFungibleTokensRequest(ApiRequest.expireCache);
    };
  };

  let useResetAllAccountsTokens = () => {
    let store = useStoreContext();
    let (tokensRequests, setTokensRequests) =
      store.accountsTokensRequestState;
    () => setTokensRequests(_ => resetRequests(tokensRequests));
  };

  let useCreate = () => {
    let resetTokens = useResetAll();
    TokensApiRequest.useCreate(~sideEffect=_ => resetTokens(), ());
  };

  let useCheck = tokens => {
    TokensApiRequest.useCheckTokenContract(tokens);
  };

  let useDelete = () => {
    let resetAccounts = useResetAll();
    TokensApiRequest.useDelete(~sideEffect=_ => resetAccounts(), ());
  };
};

module Aliases = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.aliasesRequestState;
  };

  let useRequest = () => {
    let (aliasesRequest, _) = useRequestState();
    aliasesRequest;
  };

  let filterAccounts = (~aliases, ~accounts) =>
    aliases->PublicKeyHash.Map.keep((k, _) =>
      !accounts->PublicKeyHash.Map.has(k)
    );

  let useRequestExceptAccounts = () => {
    let store = useStoreContext();
    let (aliasesRequest, _) = store.aliasesRequestState;
    let (accountsRequest, _) = store.accountsRequestState;

    switch (aliasesRequest, accountsRequest) {
    | (ApiRequest.Done(Ok(aliases), t), Done(Ok(accounts), t')) =>
      ApiRequest.Done(Ok(filterAccounts(~aliases, ~accounts)), min(t, t'))

    | (Loading(Some(aliases)), Loading(Some(accounts))) =>
      Loading(Some(filterAccounts(~aliases, ~accounts)))

    | (Loading(Some(aliases)), Done(Ok(accounts), _))
    | (Done(Ok(aliases), _), Loading(Some(accounts))) =>
      Loading(Some(filterAccounts(~aliases, ~accounts)))
    | (Done(Error(e), t), _)
    | (_, Done(Error(e), t)) => Done(Error(e), t)

    | (Loading(None), _)
    | (_, Loading(None)) => Loading(None)
    | (NotAsked, _)
    | (_, NotAsked) => NotAsked
    };
  };

  let useResetAll = () => {
    let (_, setAliasesRequest) = useRequestState();
    () => setAliasesRequest(ApiRequest.expireCache);
  };

  let useGetAll = () => {
    let aliasesRequest = useRequest();
    aliasesRequest
    ->ApiRequest.getDoneOk
    ->Option.getWithDefault(PublicKeyHash.Map.empty);
  };

  let useCreate = () => {
    let resetAliases = useResetAll();
    AliasApiRequest.useCreate(~sideEffect=_ => resetAliases(), ());
  };

  let useUpdate = () => {
    let resetAliases = useResetAll();
    AliasApiRequest.useUpdate(~sideEffect=_ => resetAliases(), ());
  };

  let useDelete = () => {
    let resetAliases = useResetAll();
    AliasApiRequest.useDelete(~sideEffect=_ => resetAliases(), ());
  };
};

module Accounts = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.accountsRequestState;
  };

  let useRequest = () => {
    let (accountsRequest, _) = useRequestState();
    accountsRequest;
  };

  let useGetAll = () => {
    let accountsRequest = useRequest();
    accountsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty);
  };

  let useGetAllWithDelegates = () => {
    let accounts = useGetAll();
    let delegates = Delegate.useGetAll();

    accounts->PublicKeyHash.Map.map(account => {
      let delegate = delegates->(Map.String.get((account.address :> string)));
      (account, delegate);
    });
  };

  let useGetFromAddress = (address: PublicKeyHash.t) => {
    let accounts = useGetAll();
    accounts->PublicKeyHash.Map.get(address);
  };

  let useGetFromOptAddress = (address: option(PublicKeyHash.t)) => {
    let accounts = useGetAll();
    address->Option.flatMap(address =>
      accounts->PublicKeyHash.Map.get(address)
    );
  };

  let useResetNames = () => {
    let resetAliases = Aliases.useResetAll();
    let resetOperations = Operations.useResetNames();
    let (_, setAccountsRequest) = useRequestState();
    () => {
      setAccountsRequest(ApiRequest.expireCache);
      resetAliases();
      resetOperations();
    };
  };

  let useResetAll = () => {
    let resetOperations = Operations.useResetAll();
    let resetAliases = Aliases.useResetAll();
    let (_, setAccountsRequest) = useRequestState();
    () => {
      setAccountsRequest(ApiRequest.expireCache);
      resetOperations();
      resetAliases();
    };
  };

  let useUpdate = () => {
    let resetAccounts = useResetNames();
    AccountApiRequest.useUpdate(~sideEffect=_ => resetAccounts(), ());
  };

  let useDelete = () => {
    let resetAccounts = useResetAll();
    AccountApiRequest.useDelete(~sideEffect=_ => resetAccounts(), ());
  };
};

module Secrets = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.secretsRequestState;
  };

  let useGetAll = () => {
    let (secretsRequest, _) = useRequestState();
    secretsRequest->ApiRequest.getWithDefault([||]);
  };

  let useLoad = () => {
    let requestState = useRequestState();
    SecretApiRequest.useLoad(requestState);
  };

  let useGetRecoveryPhrase = (~index: int) => {
    let requestState = React.useState(() => ApiRequest.NotAsked);
    SecretApiRequest.useGetRecoveryPhrase(~requestState, ~index);
  };

  let useResetNames = () => {
    let (_, setSecretsRequest) = useRequestState();
    () => {
      setSecretsRequest(ApiRequest.expireCache);
    };
  };

  let useResetAll = () => {
    let resetAccounts = Accounts.useResetAll();
    let (_, setSecretsRequest) = useRequestState();
    () => {
      setSecretsRequest(ApiRequest.expireCache);
      resetAccounts();
    };
  };

  let useCreateWithMnemonics = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useCreateWithMnemonics(
      ~sideEffect=_ => resetSecrets(),
      (),
    );
  };

  let useMnemonicImportKeys = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useMnemonicImportKeys(
      ~sideEffect=_ => resetSecrets(),
      (),
    );
  };

  let useLedgerImport = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useLedgerImport(~sideEffect=_ => resetSecrets(), ());
  };

  let useLedgerScan = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useLedgerScan(~sideEffect=_ => resetSecrets(), ());
  };

  let useDerive = () => {
    let resetAccounts = useResetAll();
    SecretApiRequest.useDerive(~sideEffect=_ => resetAccounts(), ());
  };

  let useUpdate = () => {
    let resetSecrets = useResetNames();
    SecretApiRequest.useUpdate(~sideEffect=_ => resetSecrets(), ());
  };

  let useDelete = () => {
    let resetAccounts = useResetAll();
    SecretApiRequest.useDelete(~sideEffect=_ => resetAccounts(), ());
  };
};

module SelectedAccount = {
  let useGet = () => {
    let store = useStoreContext();
    let accounts = Accounts.useGetAll();

    let (selected, set) = store.selectedAccountState;
    let selected =
      selected->Option.flatMap(pkh => accounts->PublicKeyHash.Map.get(pkh));

    switch (selected) {
    | Some(selectedAccount) => Some(selectedAccount)
    | None =>
      accounts
      ->PublicKeyHash.Map.valuesToArray
      ->SortArray.stableSortBy(Account.compareName)
      ->Array.get(0)
      ->Option.map(v => {
          set(_ => v.Account.address->Some);
          v;
        })
    };
  };

  let useSet = () => {
    let store = useStoreContext();
    let (_, setSelectedAccount) = store.selectedAccountState;

    newAccount => setSelectedAccount(_ => Some(newAccount));
  };
};

module SelectedToken = {
  let useGet = () => {
    let store = useStoreContext();
    let tokens = Tokens.useGetAllFungible();

    /// FIXME: this is clearly a bug!
    switch (store.selectedTokenState, tokens) {
    | ((Some((selectedToken, tokenId)), _), tokens) =>
      switch (tokens->TokensLibrary.Generic.getToken(selectedToken, tokenId)) {
      | Some((TokensLibrary.Token.Full(t), _)) => t->Some
      | _ => None
      }
    | _ => None
    };
  };

  let useSet = () => {
    let store = useStoreContext();
    let (_, setSelectedToken) = store.selectedTokenState;
    newToken => setSelectedToken(_ => newToken);
  };
};

module Beacon = {
  let useClient = () => {
    let store = useStoreContext();
    let (client, setClient) = store.beaconClient;
    let destroy = () =>
      switch (client) {
      | Some(client) =>
        client
        ->ReBeacon.WalletClient.destroy
        // after a call to destroy client is no more usable we need to create a new one
        ->Promise.getOk(_ =>
            setClient(_ => Some(BeaconApiRequest.makeClient()))
          )
      | None => ()
      };

    (client, destroy);
  };

  let useNextRequestState = () => {
    let store = useStoreContext();
    store.beaconNextRequestState;
  };

  module Peers = {
    let useRequestState = () => {
      let store = useStoreContext();
      store.beaconPeersRequestState;
    };

    let useResetAll = () => {
      let (_, setBeaconPeersRequest) = useRequestState();
      () => setBeaconPeersRequest(ApiRequest.expireCache);
    };

    let useGetAll = () => {
      let (client, _) = useClient();
      let beaconPeersRequestState = useRequestState();
      BeaconApiRequest.Peers.useLoad(client, beaconPeersRequestState);
    };

    let useDelete = () => {
      let (client, _) = useClient();
      let resetBeaconPeers = useResetAll();
      BeaconApiRequest.Peers.useDelete(
        ~client,
        ~sideEffect=_ => resetBeaconPeers(),
        (),
      );
    };
  };

  module Permissions = {
    let useRequestState = () => {
      let store = useStoreContext();
      store.beaconPermissionsRequestState;
    };

    let useResetAll = () => {
      let (_, setBeaconPermissionsRequest) = useRequestState();
      () => setBeaconPermissionsRequest(ApiRequest.expireCache);
    };

    let useGetAll = () => {
      let (client, _) = useClient();
      let beaconPermissionsRequestState = useRequestState();
      BeaconApiRequest.Permissions.useLoad(
        client,
        beaconPermissionsRequestState,
      );
    };

    let useDelete = () => {
      let (client, _) = useClient();
      let resetBeaconPermissions = useResetAll();
      BeaconApiRequest.Permissions.useDelete(
        ~client,
        ~sideEffect=_ => resetBeaconPermissions(),
        (),
      );
    };
  };
};
