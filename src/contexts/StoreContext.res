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

type reactState<'state> = ('state, ('state => 'state) => unit)

type error = Errors.t

type requestsState<'requestResponse> = Map.String.t<ApiRequest.t<'requestResponse>>

type requestState<'requestResponse> = reactState<ApiRequest.t<'requestResponse>>

type apiRequestsState<'requestResponse> = reactState<requestsState<'requestResponse>>

type nextState<'value> = (unit => option<'value>, unit => unit)

type state = {
  selectedAccountState: reactState<option<PublicKeyHash.t>>,
  selectedTokenState: reactState<option<(PublicKeyHash.t, TokenRepr.kind)>>,
  accountsRequestState: requestState<PublicKeyHash.Map.map<Account.t>>,
  multisigsRequestState: requestState<PublicKeyHash.Map.map<Multisig.t>>,
  secretsRequestState: reactState<ApiRequest.t<array<Secret.derived>>>,
  balanceRequestsState: requestState<PublicKeyHash.Map.map<Tez.t>>,
  delegateRequestsState: apiRequestsState<option<PublicKeyHash.t>>,
  delegateInfoRequestsState: apiRequestsState<option<NodeAPI.Delegate.delegationInfo>>,
  operationsRequestsState: apiRequestsState<OperationApiRequest.operationsResponse>,
  pendingOperationsRequestsState: apiRequestsState<
    ReBigNumber.Map.t<ReBigNumber.Map.key, Multisig.API.PendingOperation.t, ReBigNumber.Map.id>,
  >,
  aliasesRequestState: reactState<ApiRequest.t<PublicKeyHash.Map.map<Alias.t>>>,
  bakersRequestState: reactState<ApiRequest.t<array<Delegate.t>>>,
  tokensRequestState: reactState<ApiRequest.t<TokensApiRequest.Fungible.fetched>>,
  fungibleTokensRequestState: reactState<ApiRequest.t<TokensApiRequest.Fungible.fetched>>,
  tokensRegistryRequestState: reactState<ApiRequest.t<TokensApiRequest.registry>>,
  accountsTokensRequestState: apiRequestsState<TokensApiRequest.NFT.fetched>,
  accountsTokensNumberRequestState: apiRequestsState<int>,
  tokensNumberRequestState: requestState<int>,
  balanceTokenRequestsState: requestState<TokenRepr.Map.map<TokenRepr.Unit.t>>,
  chainIdState: reactState<option<Network.chainId>>,
  eulaSignatureRequestState: reactState<bool>,
  beaconPeersRequestState: requestState<array<Beacon.peerInfo>>,
  beaconClient: reactState<option<ReBeacon.WalletClient.t>>,
  beaconPermissionsRequestState: reactState<ApiRequest.t<array<Beacon.permissionInfo>>>,
  beaconNextRequestState: nextState<Beacon.Message.Request.t>,
}

// Context and Provider

let initialApiRequestsState = (Map.String.empty, _ => ())

let initialState = {
  selectedAccountState: (None, _ => ()),
  selectedTokenState: (None, _ => ()),
  accountsRequestState: (NotAsked, _ => ()),
  multisigsRequestState: (NotAsked, _ => ()),
  secretsRequestState: (NotAsked, _ => ()),
  balanceRequestsState: (NotAsked, _ => ()),
  delegateRequestsState: initialApiRequestsState,
  delegateInfoRequestsState: initialApiRequestsState,
  operationsRequestsState: initialApiRequestsState,
  pendingOperationsRequestsState: initialApiRequestsState,
  aliasesRequestState: (NotAsked, _ => ()),
  bakersRequestState: (NotAsked, _ => ()),
  tokensRequestState: (NotAsked, _ => ()),
  fungibleTokensRequestState: (NotAsked, _ => ()),
  tokensRegistryRequestState: (NotAsked, _ => ()),
  accountsTokensRequestState: initialApiRequestsState,
  accountsTokensNumberRequestState: initialApiRequestsState,
  tokensNumberRequestState: (NotAsked, _ => ()),
  balanceTokenRequestsState: (NotAsked, _ => ()),
  chainIdState: (None, _ => ()),
  eulaSignatureRequestState: (false, _ => ()),
  beaconClient: (None, _ => ()),
  beaconPeersRequestState: (NotAsked, _ => ()),
  beaconPermissionsRequestState: (NotAsked, _ => ()),
  beaconNextRequestState: (() => None, () => ()),
}

let context = React.createContext(initialState)

module Provider = {
  let makeProps = (~value, ~children, ()) =>
    {
      "value": value,
      "children": children,
    }

  let make = React.Context.provider(context)
}

let useStoreContext = () => React.useContext(context)

let extractAddresses = x =>
  x->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)->PublicKeyHash.Map.keysToList

let useLoadBalances = (~forceFetch=true, accounts, multisigs, balances) => {
  let requestState = balances
  let accounts = extractAddresses(accounts)
  let multisigs = extractAddresses(multisigs)
  let addresses = List.reverseConcat(multisigs, accounts)
  BalanceApiRequest.useLoadBalances(~forceFetch, ~requestState, addresses)
}
// Final Provider

@react.component
let make = (~children) => {
  let config = ConfigContext.useContent()

  let network = React.useMemo1(() => config.network, [config])
  let networkStatus = ConfigContext.useNetworkStatus()

  let selectedAccountState = React.useState(() => None)

  let (selectedAccount, setSelectedAccount) = selectedAccountState

  let selectedTokenState = React.useState(() => None)

  let (selectedToken, _) = selectedTokenState

  let accountsRequestState = React.useState(() => ApiRequest.NotAsked)
  let (
    accountsRequest: ApiRequest.t<PublicKeyHash.Map.map<_>>,
    _setAccountsRequest,
  ) = accountsRequestState

  let multisigsRequestState = React.useState(() => ApiRequest.NotAsked)

  let balanceRequestsState = React.useState(() => ApiRequest.NotAsked)
  let delegateRequestsState = React.useState(() => Map.String.empty)
  let delegateInfoRequestsState = React.useState(() => Map.String.empty)
  let operationsRequestsState = React.useState(() => Map.String.empty)
  let pendingOperationsRequestsState = React.useState(() => Map.String.empty)
  let accountsTokensRequestState = React.useState(() => Map.String.empty)
  let accountsTokensNumberRequestState = React.useState(() => Map.String.empty)
  let tokensNumberRequestState = React.useState(() => ApiRequest.NotAsked)
  let balanceTokenRequestsState = React.useState(() => ApiRequest.NotAsked)
  let chainIdState = React.useState(() => None)
  let aliasesRequestState = React.useState(() => ApiRequest.NotAsked)
  let bakersRequestState = React.useState(() => ApiRequest.NotAsked)
  let tokensRequestState = React.useState(() => ApiRequest.NotAsked)
  let fungibleTokensRequestState = React.useState(() => ApiRequest.NotAsked)
  let tokensRegistryRequestState = React.useState(() => ApiRequest.NotAsked)
  let secretsRequestState = React.useState(() => ApiRequest.NotAsked)

  let beaconClient = React.useState(() => Some(BeaconApiRequest.makeClient()))
  let beaconPeersRequestState = React.useState(() => ApiRequest.NotAsked)
  let beaconPermissionsRequestState = React.useState(() => ApiRequest.NotAsked)
  let beaconNextRequestState = BeaconApiRequest.useNextRequestState(
    beaconClient,
    beaconPeersRequestState,
  )

  let (_, setEulaSignature) as eulaSignatureRequestState = React.useState(() => false)

  let _: ApiRequest.t<_> = SecretApiRequest.useLoad(secretsRequestState)
  let accounts: ApiRequest.t<_> = AccountApiRequest.useLoad(accountsRequestState)
  let multisigs: ApiRequest.t<_> = MultisigApiRequest.useLoad(multisigsRequestState)
  let _: ApiRequest.t<_> = AliasApiRequest.useLoad(aliasesRequestState)
  let _: ApiRequest.t<_> = TokensApiRequest.useLoadTokensFromCache(tokensRequestState, #FT)

  React.useEffect0(() => {
    setEulaSignature(_ => Disclaimer.needSigning())
    None
  })

  let resetNetwork = () => {
    snd(multisigsRequestState)(ApiRequest.expireCache)
    snd(balanceRequestsState)(ApiRequest.expireCache)
    snd(balanceTokenRequestsState)(ApiRequest.expireCache)
    snd(selectedTokenState)(_ => None)
    snd(chainIdState)(_ => None)
  }

  React.useEffect1(() => {
    networkStatus.previous == Some(Offline) && networkStatus.current == Online ? resetNetwork() : ()
    None
  }, [networkStatus])

  React.useEffect1(() => {
    resetNetwork()
    None
  }, [network.chain])

  React.useEffect1(() => {
    let setBalancesToken = snd(balanceTokenRequestsState)
    setBalancesToken(ApiRequest.expireCache)
    None
  }, [selectedToken])

  // Select a default account if no one selected
  React.useEffect2(() => {
    if selectedAccount->Option.isNone {
      accountsRequest
      ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
      ->PublicKeyHash.Map.valuesToArray
      ->Array.get(0)
      ->Option.iter((account: Account.t) => setSelectedAccount(_ => Some(account.address)))
    }
    None
  }, (accountsRequest, selectedAccount))

  // When multisigs or implicit accounts are updated,
  // aliases should be updated as well.
  React.useEffect2(() => {
    snd(aliasesRequestState)(ApiRequest.expireCache)
    None
  }, (fst(accountsRequestState), fst(multisigsRequestState)))

  let _ = useLoadBalances(accounts, multisigs, balanceRequestsState)

  <Provider
    value={
      selectedAccountState: selectedAccountState,
      selectedTokenState: selectedTokenState,
      accountsRequestState: accountsRequestState,
      multisigsRequestState: multisigsRequestState,
      secretsRequestState: secretsRequestState,
      balanceRequestsState: balanceRequestsState,
      delegateRequestsState: delegateRequestsState,
      delegateInfoRequestsState: delegateInfoRequestsState,
      operationsRequestsState: operationsRequestsState,
      pendingOperationsRequestsState: pendingOperationsRequestsState,
      aliasesRequestState: aliasesRequestState,
      bakersRequestState: bakersRequestState,
      tokensRequestState: tokensRequestState,
      fungibleTokensRequestState: fungibleTokensRequestState,
      tokensRegistryRequestState: tokensRegistryRequestState,
      accountsTokensRequestState: accountsTokensRequestState,
      accountsTokensNumberRequestState: accountsTokensNumberRequestState,
      tokensNumberRequestState: tokensNumberRequestState,
      balanceTokenRequestsState: balanceTokenRequestsState,
      chainIdState: chainIdState,
      eulaSignatureRequestState: eulaSignatureRequestState,
      beaconClient: beaconClient,
      beaconNextRequestState: beaconNextRequestState,
      beaconPeersRequestState: beaconPeersRequestState,
      beaconPermissionsRequestState: beaconPermissionsRequestState,
    }>
    children
  </Provider>
}

// Hooks

// Utils

let useRequestsState = (getRequestsState, key: option<string>) => {
  let store = useStoreContext()
  let (requests, setRequests) = store->getRequestsState

  let request = React.useMemo2(
    () =>
      key->Option.mapWithDefault(ApiRequest.NotAsked, key =>
        requests->Map.String.get(key)->Option.getWithDefault(ApiRequest.NotAsked)
      ),
    (key, requests),
  )

  let setRequest = React.useCallback2(
    newRequestSetter =>
      key->Option.iter(key =>
        setRequests((request: requestsState<_>) =>
          request->Map.String.update(key, (oldRequest: option<ApiRequest.t<_>>) => Some(
            newRequestSetter(oldRequest->Option.getWithDefault(NotAsked)),
          ))
        )
      ),
    (key, setRequests),
  )

  (request, setRequest)
}

let useGetRequestStateFromMap = useRequestsState

let resetRequests = requestsState => requestsState->Map.String.map(ApiRequest.expireCache)

let resetRequest = requestState => requestState->ApiRequest.expireCache

let useChainId = () => {
  let store = useStoreContext()
  store.chainIdState->fst
}

let useEulaSignature = () => {
  let store = useStoreContext()
  store.eulaSignatureRequestState->fst
}

let setEulaSignature = () => {
  let store = useStoreContext()
  store.eulaSignatureRequestState->snd
}

module Balance = {
  let useAll = forceFetch => {
    let store = useStoreContext()
    useLoadBalances(
      ~forceFetch,
      store.accountsRequestState->fst,
      store.multisigsRequestState->fst,
      store.balanceRequestsState,
    )
  }

  let useOne = (request, address: PublicKeyHash.t) => {
    BalanceApiRequest.getOne(request, address)
  }

  let handleBalance = (map: PublicKeyHash.Map.map<Tez.t>) =>
    map->PublicKeyHash.Map.reduce(Tez.zero, (a, _, c) => {
      open Tez.Infix
      a + c
    })

  let handleBalances = (accountsSize, requests, reduce) => {
    let allErrors = () => requests->Array.every(ApiRequest.isError)
    let getAllDoneOk = () => requests->Array.keepMap(ApiRequest.getDoneOk)

    if allErrors() {
      ApiRequest.Done(Error(BalanceApiRequest.EveryBalancesFail), Expired)
    } else {
      let allDone = getAllDoneOk()
      if allDone->Array.size == accountsSize {
        let total = allDone->reduce
        Done(Ok(total), ApiRequest.initCache())
      } else {
        Loading(None)
      }
    }
  }

  let useGetTotal = () => {
    let store = useStoreContext()
    let (balanceRequests, _) = store.balanceRequestsState
    let requests = balanceRequests
    ApiRequest.map(requests, handleBalance)
  }

  let useResetAll = () => {
    let store = useStoreContext()
    let (_, setBalanceRequests) = store.balanceRequestsState
    () => setBalanceRequests(resetRequest)
  }
}

module BalanceToken = {
  let useOne = (
    request,
    token: PublicKeyHash.t,
    kind: TokenRepr.kind,
    ~address: PublicKeyHash.t,
  ) => {
    let mapKey = switch kind {
    | TokenRepr.FA1_2 => (address, (token, None))
    | FA2(tokenId) => (address, (token, Some(tokenId)))
    }
    TokensApiRequest.getOneBalance(request, mapKey)
  }

  let handleBalance = (selectedToken, map: TokenRepr.Map.map<TokenRepr.Unit.t>) =>
    map->TokenRepr.Map.reduce(TokenRepr.Unit.zero, (a, (_, b), c) =>
      switch selectedToken {
      | Some((token, kind)) =>
        let id = switch kind {
        | TokenRepr.FA1_2 => None
        | FA2(tokenId) => Some(tokenId)
        }
        if b == (token, id) {
          open TokenRepr.Unit.Infix
          a + c
        } else {
          a
        }
      | None => TokenRepr.Unit.zero
      }
    )

  let useGetTotal = () => {
    let store = useStoreContext()
    let (balanceRequests, _) = store.balanceTokenRequestsState
    let (selectedToken, _) = store.selectedTokenState
    let requests = balanceRequests
    ApiRequest.map(requests, handleBalance(selectedToken))
  }

  let useResetAll = () => {
    let store = useStoreContext()
    let (_, setBalanceTokenRequests) = store.balanceTokenRequestsState
    () => setBalanceTokenRequests(resetRequest)
  }
}

module Delegate = {
  let useRequestState = useRequestsState(store => store.delegateRequestsState)

  let useLoad = (address: PublicKeyHash.t) => {
    let requestState = useRequestState(Some((address :> string)))

    DelegateApiRequest.useLoad(~requestState, ~address)
  }

  let useGetAll = () => {
    let store = useStoreContext()
    let (delegateRequests, _) = store.delegateRequestsState

    delegateRequests
    ->Map.String.map(request => request->ApiRequest.getDoneOk->Option.flatMap(v => v))
    ->Map.String.keep((_k, v) => v->Option.isSome)
    ->Map.String.map(Option.getExn)
  }

  let useGetAllRequests = () => {
    let store = useStoreContext()
    let (delegateRequests, _) = store.delegateRequestsState

    delegateRequests
  }
}

module DelegateInfo = {
  let useRequestState = useRequestsState(store => store.delegateInfoRequestsState)

  let useLoad = (address: PublicKeyHash.t) => {
    let requestState = useRequestState(Some((address :> string)))

    DelegateApiRequest.useLoadInfo(~requestState, ~address)
  }

  let useResetAll = () => {
    let store = useStoreContext()
    let (_, setDelegateRequests) = store.delegateRequestsState
    let (_, setDelegateInfoRequests) = store.delegateInfoRequestsState
    () => {
      setDelegateRequests(resetRequests)
      setDelegateInfoRequests(resetRequests)
    }
  }
}

module Operations = {
  let useRequestState = useRequestsState(store => store.operationsRequestsState)

  let useLoad = (~limit=?, ~types=?, ~address: PublicKeyHash.t, ()) => {
    let requestState = useRequestState((address->Some :> option<string>))

    OperationApiRequest.useLoad(~requestState, ~limit?, ~types?, ~address, ())
  }

  let useResetNames = () => {
    let store = useStoreContext()

    let (_, setOperationsRequests) = store.operationsRequestsState
    () => setOperationsRequests(resetRequests)
  }

  let useResetAll = () => {
    let store = useStoreContext()
    let resetBalances = Balance.useResetAll()
    let resetBalanceTokens = BalanceToken.useResetAll()
    let resetDelegatesAndDelegatesInfo = DelegateInfo.useResetAll()
    let (_, setOperationsRequests) = store.operationsRequestsState
    () => {
      setOperationsRequests(resetRequests)
      resetBalances()
      resetBalanceTokens()
      resetDelegatesAndDelegatesInfo()
    }
  }

  let useCreate = () => {
    let resetOperations = useResetAll()
    let settings = ConfigContext.useContent()
    let sideEffect = (result: ReTaquito.Toolkit.Operation.result) =>
      OperationApiRequest.waitForConfirmation(settings, result.hash)->Promise.get(_ =>
        resetOperations()
      )
    OperationApiRequest.useCreate(~sideEffect, ())
  }

  let useSimulate = () => {
    let (operationSimulateRequest, sendOperationSimulate) = OperationApiRequest.useSimulate()
    let sendOperationSimulate = (source, managers) => sendOperationSimulate((source, managers))
    (operationSimulateRequest, sendOperationSimulate)
  }
}

module Bakers = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.bakersRequestState
  }

  let useLoad = () => {
    let requestState = useRequestState()

    DelegateApiRequest.useLoadBakers(~requestState)
  }
}

module Tokens = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.tokensRequestState
  }

  let useFungibleRequestState = () => {
    let store = useStoreContext()
    store.fungibleTokensRequestState
  }

  let useGetTokens = (requestState, filter) =>
    TokensApiRequest.useLoadTokensFromCache(
      requestState,
      filter,
    )->ApiRequest.mapWithDefault(TokensLibrary.Contracts.empty, x =>
      switch x {
      | #Cached(tokens) => tokens
      // technically impossible, since useLoadTokens only returns `Cached,
      // but this is due to the unification with the request
      | #Fetched(tokens, _) => tokens
      }
    )

  let useGetAll = () => {
    let tokensRequestState = useRequestState()
    useGetTokens(tokensRequestState, #Any)
  }

  let useGetAllFungible = () => {
    let fungibleTokensRequestState = useFungibleRequestState()
    useGetTokens(fungibleTokensRequestState, #FT)
  }

  let useAccountTokensNumber = (account: PublicKeyHash.t) => {
    let useRequestsState = useGetRequestStateFromMap(store =>
      store.accountsTokensNumberRequestState
    )

    let accountsTokensRequestState = useRequestsState((Some(account) :> option<string>))

    TokensApiRequest.NFT.useAccountTokensNumber(accountsTokensRequestState, account)
  }

  let useAccountNFTs = (onTokens, onStop, account: PublicKeyHash.t) => {
    let useRequestsState = useGetRequestStateFromMap(store => store.accountsTokensRequestState)

    let accountsTokensRequestState = useRequestsState((Some(account) :> option<string>))

    let tokensNumberRequest = useAccountTokensNumber(account)

    TokensApiRequest.NFT.useFetchWithCache(
      onTokens,
      onStop,
      accountsTokensRequestState,
      tokensNumberRequest,
    )
  }

  let useTokensNumberRequest = () => {
    let store = useStoreContext()
    store.tokensNumberRequestState
  }

  let useTokensNumber = (accounts: list<PublicKeyHash.t>) => {
    let requestState = useTokensNumberRequest()

    TokensApiRequest.Fungible.useAccountsTokensNumber(requestState, accounts)
  }

  let useFetchTokens = (onTokens, onStop, accounts: list<PublicKeyHash.t>) => {
    let requestState = useRequestState()

    let tokensNumberRequest = useTokensNumber(accounts)

    TokensApiRequest.Fungible.useFetchWithCache(onTokens, onStop, requestState, tokensNumberRequest)
  }

  let useResetAll = () => {
    let (_, setTokensRequest) = useRequestState()
    let (_, setFungibleTokensRequest) = useRequestState()
    () => {
      setTokensRequest(ApiRequest.expireCache)
      setFungibleTokensRequest(ApiRequest.expireCache)
    }
  }

  let useCreate = () => {
    let sideEffect = useResetAll()
    TokensApiRequest.useCreate(~sideEffect, ())
  }

  let useCacheToken = () => {
    let sideEffect = useResetAll()
    TokensApiRequest.useCacheToken(~sideEffect, ())
  }

  let useDelete = pruneCache => {
    let sideEffect = useResetAll()
    TokensApiRequest.useDelete(~sideEffect, pruneCache)
  }
}

module Aliases = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.aliasesRequestState
  }

  let useRequest = () => {
    let (aliasesRequest, _) = useRequestState()
    aliasesRequest
  }

  let filterAccounts = (~aliases, ~accounts, ~multisigs) =>
    aliases->PublicKeyHash.Map.keep((k, _) =>
      !(accounts->PublicKeyHash.Map.has(k) || multisigs->PublicKeyHash.Map.has(k))
    )

  let useRequestExceptAccounts = () => {
    let store = useStoreContext()
    let (aliasesRequest, _) = store.aliasesRequestState
    let (accountsRequest, _) = store.accountsRequestState
    let (multisigsRequest, _) = store.multisigsRequestState

    switch (aliasesRequest, accountsRequest, multisigsRequest) {
    | (ApiRequest.Done(Ok(aliases), t), Done(Ok(accounts), t'), Done(Ok(multisigs), t'')) =>
      ApiRequest.Done(Ok(filterAccounts(~aliases, ~accounts, ~multisigs)), min(t, min(t', t'')))

    | (
        Loading(Some(aliases)) | Done(Ok(aliases), _),
        Loading(Some(accounts)) | Done(Ok(accounts), _),
        Loading(Some(multisigs)) | Done(Ok(multisigs), _),
      ) =>
      Loading(Some(filterAccounts(~aliases, ~accounts, ~multisigs)))

    | (Done(Error(e), t), _, _)
    | (_, Done(Error(e), t), _)
    | (_, _, Done(Error(e), t)) =>
      Done(Error(e), t)

    | (Loading(None), _, _)
    | (_, Loading(None), _)
    | (_, _, Loading(None)) =>
      Loading(None)
    | (NotAsked, _, _)
    | (_, NotAsked, _)
    | (_, _, NotAsked) =>
      NotAsked
    }
  }

  let useResetAll = () => {
    let (_, setAliasesRequest) = useRequestState()
    () => setAliasesRequest(ApiRequest.expireCache)
  }

  let useGetAll = () => {
    let aliasesRequest = useRequest()
    aliasesRequest->ApiRequest.getDoneOk->Option.getWithDefault(PublicKeyHash.Map.empty)
  }

  let useCreate = () => {
    let sideEffect = useResetAll()
    AliasApiRequest.useCreate(~sideEffect, ())
  }

  let useUpdate = () => {
    let sideEffect = useResetAll()
    AliasApiRequest.useUpdate(~sideEffect, ())
  }

  let useDelete = () => {
    let sideEffect = useResetAll()
    AliasApiRequest.useDelete(~sideEffect, ())
  }
}

module Contract = {
  type t = Token(TokenContract.kind) | Multisig(Multisig.t) | Unknown

  let useCheck = tokens => {
    let set = (~config: ConfigContext.env, address) => {
      switch tokens->TokensLibrary.Generic.pickAnyAtAddress(address) {
      | None =>
        config.network
        ->NodeAPI.Tokens.checkTokenContract(address)
        ->Promise.flatMapOk(x =>
          switch x {
          | #NotAToken =>
            Multisig.API.getOneFromNetwork(config.network, address)->Promise.map(r =>
              switch r {
              | Ok(multisig) => Ok(Multisig(multisig))
              | Error(_) => Ok(Unknown)
              }
            )
          | #...TokenContract.kind as kind => Token(kind)->Promise.ok
          }
        )
      | Some((_, _, (token, _))) => Token(token->TokensLibrary.Token.kind)->Promise.ok
      }
    }
    ApiRequest.useSetter(~set, ~kind=Logs.Contract, ~toast=false, ())
  }
}

module Multisig_ = Multisig
module Multisig_API = Multisig.API

module Multisig = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.multisigsRequestState
  }

  let useRequest = () => {
    let (multisigsRequest, _) = useRequestState()
    multisigsRequest
  }

  let useCreate = () => {
    ()
  }

  let useGetAll = () => {
    let config = ConfigContext.useContent()
    let multisigsRequest = useRequest()
    multisigsRequest->ApiRequest.getWithDefault(
      Multisig.API.getAllFromCache(config.network)->Result.getWithDefault(PublicKeyHash.Map.empty),
    )
  }

  let useGetFromAddress = () => {
    let multisigs = useGetAll()
    (address: PublicKeyHash.t) => multisigs->PublicKeyHash.Map.get(address)
  }

  let useResetAll = () => {
    let (_, setMultisigRequest) = useRequestState()
    () => setMultisigRequest(ApiRequest.expireCache)
  }

  let useUpdate = message => {
    let sideEffect = useResetAll()
    MultisigApiRequest.useUpdate(message)(~sideEffect, ())
  }

  let useOriginate = () => useUpdate(I18n.multisig_originated)
  let useEdit = () => useUpdate(I18n.multisig_updated)
  let useRegister = () => useUpdate(I18n.multisig_registered)

  let useDelete = () => {
    let sideEffect = useResetAll()
    MultisigApiRequest.useDelete(~sideEffect, ())
  }

  module PendingOperations = {
    let useRequestState = useRequestsState(store => store.pendingOperationsRequestsState)

    let usePendingOperations = (~address: PublicKeyHash.t) => {
      let requestState = useRequestState((address->Some :> option<string>))
      MultisigApiRequest.usePendingOperations(~requestState, ~address)
    }

    let useResetAll = () => {
      let store = useStoreContext()
      let (_, setPendingOperationsRequests) = store.pendingOperationsRequestsState
      () => {
        setPendingOperationsRequests(resetRequests)
      }
    }
  }
}

module Accounts = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.accountsRequestState
  }

  let useRequest = () => {
    let (accountsRequest, _) = useRequestState()
    accountsRequest
  }

  let useGetAll = () => {
    let accountsRequest = useRequest()
    accountsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
  }

  let useGetAllWithDelegates = () => {
    let accounts = useGetAll()
    let delegates = Delegate.useGetAll()

    accounts->PublicKeyHash.Map.map(account => {
      let delegate = delegates->Map.String.get((account.address :> string))
      (account, delegate)
    })
  }

  let useGetFromAddress = (address: PublicKeyHash.t) => {
    let accounts = useGetAll()
    accounts->PublicKeyHash.Map.get(address)
  }

  let useResetNames = () => {
    let resetOperations = Operations.useResetNames()
    let (_, setAccountsRequest) = useRequestState()
    () => {
      setAccountsRequest(ApiRequest.expireCache)
      resetOperations()
    }
  }

  let useResetAll = () => {
    let resetOperations = Operations.useResetAll()
    let (_, setAccountsRequest) = useRequestState()
    () => {
      setAccountsRequest(ApiRequest.expireCache)
      resetOperations()
    }
  }

  let useUpdate = () => {
    let sideEffect = useResetNames()
    AccountApiRequest.useUpdate(~sideEffect, ())
  }

  let useDelete = () => {
    let sideEffect = useResetAll()
    AccountApiRequest.useDelete(~sideEffect, ())
  }

  let useCustomAuthLogin = () => {
    let sideEffect = useResetAll()
    CustomAuthApiRequest.useLogin(~sideEffect=_ => sideEffect(), ())
  }
}

module Secrets = {
  let useRequestState = () => {
    let store = useStoreContext()
    store.secretsRequestState
  }

  let useGetAll = () => {
    let (secretsRequest, _) = useRequestState()
    secretsRequest->ApiRequest.getWithDefault([])
  }

  let useLoad = () => {
    let requestState = useRequestState()
    SecretApiRequest.useLoad(requestState)
  }

  let useGetEncryptedRecoveryPhrase = (~index: int) => {
    let requestState = React.useState(() => ApiRequest.NotAsked)
    SecretApiRequest.useGetEncryptedRecoveryPhrase(~requestState, ~index)
  }

  let useGetRecoveryPhrase = (~index: int) => {
    let requestState = React.useState(() => ApiRequest.NotAsked)
    SecretApiRequest.useGetRecoveryPhrase(~requestState, ~index)
  }

  let useResetNames = () => {
    let (_, setSecretsRequest) = useRequestState()
    () => setSecretsRequest(ApiRequest.expireCache)
  }

  let useResetAll = () => {
    let resetMultisigs = Multisig.useResetAll()
    let resetAccounts = Accounts.useResetAll()
    let (_, setSecretsRequest) = useRequestState()
    () => {
      setSecretsRequest(ApiRequest.expireCache)
      resetAccounts()
      resetMultisigs()
    }
  }

  let useCreateWithMnemonics = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useCreateWithMnemonics(~sideEffect, ())
  }

  let useCreateFromBackupFile = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useCreateFromBackupFile(~sideEffect, ())
  }

  let useMnemonicImportKeys = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useMnemonicImportKeys(~sideEffect=_ => sideEffect(), ())
  }

  let useLedgerImport = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useLedgerImport(~sideEffect=_ => sideEffect(), ())
  }

  let useLedgerScan = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useLedgerScan(~sideEffect=_ => sideEffect(), ())
  }

  let useDerive = () => {
    let sideEffect = useResetAll()
    SecretApiRequest.useDerive(~sideEffect=_ => sideEffect(), ())
  }

  let useUpdate = () => {
    let sideEffect = useResetNames()
    SecretApiRequest.useUpdate(~sideEffect, ())
  }

  let useDelete = () => {
    let config = ConfigContext.useContent()

    let networks = Array.concat(
      Network.nativeChains->List.toArray->Array.map(x => x->fst->Network.getNetwork),
      config.customNetworks,
    )

    let sideEffect = useResetAll()
    let (request, setRequest) = SecretApiRequest.useDelete(~sideEffect=_ => sideEffect(), ())
    (
      request,
      i => {
        setRequest(i)
        // Remove from cache multisigs that:
        // - are related to one of the removed pkh
        // - AND have the default generated name (assume it has been automatically added by umami)
        // If another remaining secret is related to any deleted multisig, the multisig
        // will be re-added with the same name so it is ok to remove it here
        ->Promise.flatMapOk(addresses =>
          Promise.allArray(
            networks->Array.map(network =>
              Multisig_API.getAddresses(network, ~addresses)->Promise.flatMapOk(contracts =>
                Multisig_API.get(network, contracts)
              )
            ),
          )
        )
        ->Promise.mapOk(contracts =>
          Array.reduce(contracts, PublicKeyHash.Map.empty, (acc, r) =>
            switch r {
            | Result.Ok(map) =>
              PublicKeyHash.Map.merge(acc, map, (_, a, b) => {
                let c = (a == None ? b : a)->Option.getExn
                c.alias == Multisig_API.defaultName(c.address) ? Some(c) : None
              })
            | Result.Error(_) => acc
            }
          )
        )
        ->Promise.mapOk(PublicKeyHash.Map.valuesToArray)
        ->Promise.mapOk(Js.Array.map((x: Multisig_.t) => x.address))
        ->Promise.mapOk(Multisig_API.removeFromCache)
      },
    )
  }
}

module SelectedToken = {
  let useGet = () => {
    let store = useStoreContext()
    let tokens = Tokens.useGetAllFungible()

    /// FIXME: this is clearly a bug!
    switch (store.selectedTokenState, tokens) {
    | ((Some((selectedToken, tokenId)), _), tokens) =>
      switch tokens->TokensLibrary.Generic.getToken(selectedToken, tokenId->TokenRepr.kindId) {
      | Some((TokensLibrary.Token.Full(t), _)) => t->Some
      | _ => None
      }
    | _ => None
    }
  }

  let useSet = () => {
    let store = useStoreContext()
    let (_, setSelectedToken) = store.selectedTokenState
    newToken => setSelectedToken(_ => newToken)
  }
}

module Beacon = {
  let useClient = () => {
    let store = useStoreContext()
    let (client, setClient) = store.beaconClient
    let destroy = () =>
      switch client {
      | Some(client) =>
        client
        ->ReBeacon.WalletClient.destroy
        // after a call to destroy client is no more usable we need to create a new one
        ->Promise.getOk(_ => setClient(_ => Some(BeaconApiRequest.makeClient())))
      | None => ()
      }

    (client, destroy)
  }

  let useNextRequestState = () => {
    let store = useStoreContext()
    store.beaconNextRequestState
  }

  module Peers = {
    let useRequestState = () => {
      let store = useStoreContext()
      store.beaconPeersRequestState
    }

    let useResetAll = () => {
      let (_, setBeaconPeersRequest) = useRequestState()
      () => setBeaconPeersRequest(ApiRequest.expireCache)
    }

    let useGetAll = () => {
      let (client, _) = useClient()
      let beaconPeersRequestState = useRequestState()
      BeaconApiRequest.Peers.useLoad(client, beaconPeersRequestState)
    }

    let useDelete = () => {
      let (client, _) = useClient()
      let sideEffect = useResetAll()
      BeaconApiRequest.Peers.useDelete(~client, ~sideEffect, ())
    }

    let useDeleteAll = () => {
      let (client, _) = useClient()
      let sideEffect = useResetAll()
      BeaconApiRequest.Peers.useDeleteAll(~client, ~sideEffect, ())
    }
  }

  module Permissions = {
    let useRequestState = () => {
      let store = useStoreContext()
      store.beaconPermissionsRequestState
    }

    let useResetAll = () => {
      let (_, setBeaconPermissionsRequest) = useRequestState()
      () => setBeaconPermissionsRequest(ApiRequest.expireCache)
    }

    let useGetAll = () => {
      let (client, _) = useClient()
      let beaconPermissionsRequestState = useRequestState()
      BeaconApiRequest.Permissions.useLoad(client, beaconPermissionsRequestState)
    }

    let useDelete = () => {
      let (client, _) = useClient()
      let sideEffect = useResetAll()
      BeaconApiRequest.Permissions.useDelete(~client, ~sideEffect, ())
    }

    let useDeleteAll = () => {
      let (client, _) = useClient()
      let sideEffect = useResetAll()
      BeaconApiRequest.Permissions.useDeleteAll(~client, ~sideEffect, ())
    }
  }
}

let useAliasesAccountsMultisigs = () => {
  let aliasesRequest = Aliases.useRequest()
  let accountsRequest = Accounts.useRequest()
  let multisigsRequest = Multisig.useRequest()

  switch (aliasesRequest, accountsRequest, multisigsRequest) {
  | (Done(Error(e), t), _, _) | (_, Done(Error(e), t), _) | (_, _, Done(Error(e), t)) =>
    ApiRequest.Done(Error(e), t)
  | (Done(Ok(aliases), v1), Done(Ok(accounts), v2), Done(Ok(multisigs), v3)) =>
    let cache = Ok(aliases, accounts, multisigs)
    let cacheValidity = switch (v1, v2, v3) {
    | (ValidSince(v1), ValidSince(v2), ValidSince(v3)) =>
      ApiRequest.ValidSince(Js.Math.minMany_float([v1, v2, v3]))
    | _ => ApiRequest.Expired
    }
    ApiRequest.Done(cache, cacheValidity)
  | (NotAsked, _, _) | (_, NotAsked, _) | (_, _, NotAsked) => ApiRequest.NotAsked
  | (Loading(None), Loading(None), Loading(None)) => ApiRequest.Loading(None)
  | (Loading(_), _, _) | (_, Loading(_), _) | (_, _, Loading(_)) =>
    ApiRequest.Loading(
      Some(
        aliasesRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty),
        accountsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty),
        multisigsRequest->ApiRequest.getWithDefault(PublicKeyHash.Map.empty),
      ),
    )
  }
}

module AccountsMultisigs = {
  // Return accounts and multisigs,
  // formatted as aliases or corresponding registred alias if it exists.
  let useRequest = () => {
    ApiRequest.map(useAliasesAccountsMultisigs(), ((aliases, accounts, multisigs)) => {
      let replaceName = v => {
        switch PublicKeyHash.Map.get(aliases, v.Alias.address) {
        | Some(alias) => {...v, name: alias.Alias.name}
        | None => v
        }
      }
      let acc = PublicKeyHash.Map.empty
      let acc = PublicKeyHash.Map.reduce(accounts, acc, (acc, k, v) => {
        let v = Alias.fromAccount(v)
        let v = replaceName(v)
        PublicKeyHash.Map.set(acc, k, v)
      })
      let acc = PublicKeyHash.Map.reduce(multisigs, acc, (acc, k, v) => {
        let v = Alias.fromMultisig(v)
        let v = replaceName(v)
        PublicKeyHash.Map.set(acc, k, v)
      })
      acc
    })
  }

  let useGetAll = () => {
    useRequest()
    ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
    ->PublicKeyHash.Map.keep((_, a) =>
      switch a.kind {
      | Some(Account(_)) | Some(Multisig) => true
      | _ => false
      }
    )
  }

  let useGetAllWithDelegates = () => {
    let accounts = useGetAll()
    let delegates = Delegate.useGetAll()
    accounts->PublicKeyHash.Map.map(account => {
      let delegate = delegates->Map.String.get((account.address :> string))
      (account, delegate)
    })
  }
}

let useHasPendingWaiting = () => {
  let getMultisig = Multisig.useGetFromAddress()
  let accountsRequest = Accounts.useRequest()
  let multisigsRequest = Multisig.useRequest()
  address => {
    let accounts =
      accountsRequest
      ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
      ->PublicKeyHash.Map.keysToArray
    let multisigs =
      multisigsRequest
      ->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)
      ->PublicKeyHash.Map.keysToArray
    let pkhs = Array.concat(accounts, multisigs)
    let pending =
      Multisig.PendingOperations.usePendingOperations(~address)->ApiRequest.getWithDefault(
        ReBigNumber.Map.empty,
      )
    !ReBigNumber.Map.isEmpty(pending) && {
      let m = getMultisig(address)->Option.getExn
      let threshold = ReBigNumber.toInt(m.threshold)
      let signers = Array.keep(m.signers, x => Js.Array.includes(x, pkhs))
      ReBigNumber.Map.some(pending, (_, p) => {
        Array.length(p.approvals) >= threshold ||
          Array.some(signers, x => !Js.Array.includes(x, p.approvals))
      })
    }
  }
}

// For backward compatibility. To be removed.
let useGetAccountsMultisigsAliasesAsAliases = AccountsMultisigs.useRequest

let useGetImplicitFromAlias = () => {
  let accounts = Accounts.useGetAll()
  let multisigs = Multisig.useGetAll()
  alias => {
    let getAccount = k => PublicKeyHash.Map.get(accounts, k)
    let getMultisig = k => PublicKeyHash.Map.get(multisigs, k)
    switch alias.Alias.kind {
    | Some(Account(_)) => getAccount(alias.Alias.address)
    | Some(Multisig) =>
      getMultisig(alias.Alias.address)
      ->Option.flatMap(multisig => Js.Array.find(PublicKeyHash.Map.has(accounts), multisig.signers))
      ->Option.flatMap(signer => PublicKeyHash.Map.get(accounts, signer))
    | Some(Contact) => None
    | None => None
    }
  }
}

module SelectedAccount = {
  let useGetAtInit = () => {
    let store = useStoreContext()
    let accounts =
      useGetAccountsMultisigsAliasesAsAliases()->ApiRequest.getWithDefault(PublicKeyHash.Map.empty)

    let (selected, _set) = store.selectedAccountState
    let selected = selected->Option.flatMap(pkh => accounts->PublicKeyHash.Map.get(pkh))
    switch selected {
    | Some(selectedAccount) => Some(selectedAccount)
    | None =>
      accounts->PublicKeyHash.Map.valuesToArray->SortArray.stableSortBy(Alias.compare)->Array.get(0)
    }
  }

  let useGetImplicit = () => {
    let store = useStoreContext()
    let accounts = Accounts.useGetAll()
    let (selected, _set) = store.selectedAccountState
    let selected =
      selected->Option.flatMap(pkh =>
        PublicKeyHash.isImplicit(pkh) ? accounts->PublicKeyHash.Map.get(pkh) : None
      )
    switch selected {
    | Some(_) => selected
    | None =>
      accounts
      ->PublicKeyHash.Map.valuesToArray
      ->SortArray.stableSortBy(Account.compareName)
      ->Array.get(0)
    }
  }

  let useSet = () => {
    let store = useStoreContext()
    let (_, setSelectedAccount) = store.selectedAccountState

    newAccount => setSelectedAccount(_ => Some(newAccount))
  }
}
