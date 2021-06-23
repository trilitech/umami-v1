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

open Belt;

open UmamiCommon;

type reactState('state) = ('state, ('state => 'state) => unit);

type requestsState('requestResponse, 'error) =
  Map.String.t(ApiRequest.t('requestResponse, 'error));

type error = ErrorHandler.t;

type apiRequestsStateLegacy('requestResponse) =
  reactState(requestsState('requestResponse, string));

type apiRequestsState('requestResponse) =
  reactState(requestsState('requestResponse, error));

type state = {
  selectedAccountState: reactState(option(PublicKeyHash.t)),
  selectedTokenState: reactState(option(PublicKeyHash.t)),
  accountsRequestState:
    reactState(ApiRequest.t(Map.String.t(Account.t), ErrorHandler.t)),
  secretsRequestState:
    reactState(ApiRequest.t(array(Secret.derived), ErrorHandler.t)),
  balanceRequestsState: apiRequestsState(Tez.t),
  delegateRequestsState: apiRequestsState(option(PublicKeyHash.t)),
  delegateInfoRequestsState:
    apiRequestsState(option(NodeAPI.Delegate.delegationInfo)),
  operationsRequestsState:
    apiRequestsStateLegacy(OperationApiRequest.operationsResponse),
  operationsConfirmations: reactState(Set.String.t),
  aliasesRequestState:
    reactState(ApiRequest.t(Map.String.t(Alias.t), ErrorHandler.t)),
  bakersRequestState: reactState(ApiRequest.t(array(Delegate.t), string)),
  tokensRequestState:
    reactState(ApiRequest.t(Map.String.t(Token.t), string)),
  balanceTokenRequestsState: apiRequestsState(Token.Unit.t),
  apiVersionRequestState: reactState(option(Network.apiVersion)),
  eulaSignatureRequestState: reactState(bool),
  beaconPeersRequestState:
    reactState(ApiRequest.t(array(ReBeacon.peerInfo), string)),
  beaconPermissionsRequestState:
    reactState(ApiRequest.t(array(ReBeacon.permissionInfo), string)),
};

// Context and Provider

let initialApiRequestsState = (Map.String.empty, _ => ());

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
  balanceTokenRequestsState: initialApiRequestsState,
  apiVersionRequestState: (None, _ => ()),
  eulaSignatureRequestState: (false, _ => ()),
  beaconPeersRequestState: (NotAsked, _ => ()),
  beaconPermissionsRequestState: (NotAsked, _ => ()),
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
  let settings = ConfigContext.useContent();
  let addToast = LogsContext.useToast();

  let network = settings->ConfigUtils.network;

  let selectedAccountState = React.useState(() => None);
  let (selectedAccount, setSelectedAccount) = selectedAccountState;

  let selectedTokenState = React.useState(() => None);

  let accountsRequestState = React.useState(() => ApiRequest.NotAsked);
  let (accountsRequest, _setAccountsRequest) = accountsRequestState;

  let balanceRequestsState = React.useState(() => Map.String.empty);
  let delegateRequestsState = React.useState(() => Map.String.empty);
  let delegateInfoRequestsState = React.useState(() => Map.String.empty);
  let operationsRequestsState = React.useState(() => Map.String.empty);
  let balanceTokenRequestsState = React.useState(() => Map.String.empty);
  let operationsConfirmations = React.useState(() => Set.String.empty);

  let aliasesRequestState = React.useState(() => ApiRequest.NotAsked);
  let bakersRequestState = React.useState(() => ApiRequest.NotAsked);
  let tokensRequestState = React.useState(() => ApiRequest.NotAsked);
  let secretsRequestState = React.useState(() => ApiRequest.NotAsked);
  let beaconPeersRequestState = React.useState(() => ApiRequest.NotAsked);
  let beaconPermissionsRequestState =
    React.useState(() => ApiRequest.NotAsked);

  let apiVersionRequestState = React.useState(() => None);
  let (_, setApiVersion) = apiVersionRequestState;

  let (_, setEulaSignature) as eulaSignatureRequestState =
    React.useState(() => false);

  SecretApiRequest.useLoad(secretsRequestState)->ignore;
  AccountApiRequest.useLoad(accountsRequestState)->ignore;
  AliasApiRequest.useLoad(aliasesRequestState)->ignore;
  TokensApiRequest.useLoadTokens(tokensRequestState)->ignore;

  React.useEffect0(() => {
    setEulaSignature(_ => Disclaimer.needSigning());
    None;
  });

  React.useEffect1(
    () => {
      Network.checkConfiguration(
        settings->ConfigUtils.explorer,
        settings->ConfigUtils.endpoint,
      )
      ->Future.tapOk(((v, _)) => setApiVersion(_ => Some(v)))
      ->FutureEx.getOk(((apiVersion, _)) =>
          if (!Network.checkInBound(apiVersion.Network.api)) {
            addToast(
              Logs.error(
                ~origin=Settings,
                Network.errorMsg(`APINotSupported(apiVersion.api)),
              ),
            );
          }
        );
      None;
    },
    [|network|],
  );

  // Select a default account if no one selected
  React.useEffect2(
    () => {
      if (selectedAccount->Option.isNone) {
        accountsRequest
        ->ApiRequest.getWithDefault(Map.String.empty)
        ->Map.String.valuesToArray
        ->Array.get(0)
        ->Lib.Option.iter((account: Account.t) =>
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
      balanceTokenRequestsState,
      apiVersionRequestState,
      eulaSignatureRequestState,
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
        key->Lib.Option.iter(key =>
          setRequests((request: requestsState('requestResponse, _)) =>
            request->Map.String.update(
              key, (oldRequest: option(ApiRequest.t('requestResponse, _))) =>
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

  let useGetTotal = () => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getWithDefault(Map.String.empty);

    let accountsBalanceRequests =
      accounts
      ->Map.String.valuesToArray
      ->Array.keepMap(account => {
          balanceRequests->Map.String.get((account.address :> string))
        })
      ->Array.keep(ApiRequest.isDone);

    // check if balance requests for each accounts are done
    accountsBalanceRequests->Array.size == accounts->Map.String.size
      ? Some(
          accountsBalanceRequests->Array.reduce(
            Tez.zero, (acc, balanceRequest) => {
            Tez.Infix.(
              acc
              + balanceRequest
                ->ApiRequest.getDoneOk
                ->Option.getWithDefault(Tez.zero)
            )
          }),
        )
      : None;
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
      (address: PublicKeyHash.t, tokenAddress: PublicKeyHash.t) =>
    (address :> string) ++ (tokenAddress :> string);

  let useLoad = (address: PublicKeyHash.t, tokenAddress: PublicKeyHash.t) => {
    let requestState =
      useRequestState(address->getRequestKey(tokenAddress)->Some);

    let operation =
      React.useMemo2(
        () => Token.makeGetBalance(address, tokenAddress, ()),
        (address, tokenAddress),
      );

    TokensApiRequest.useLoadOperationOffline(~requestState, ~operation);
  };

  let useGetTotal = tokenAddress => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceTokenRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getWithDefault(Map.String.empty);

    let accountsBalanceRequests =
      accounts
      ->Map.String.valuesToArray
      ->Array.keepMap(account => {
          balanceRequests->Map.String.get(
            getRequestKey(account.address, tokenAddress),
          )
        })
      ->Array.keep(ApiRequest.isDone);

    // check if balance requests for each accounts are done
    accountsBalanceRequests->Array.size == accounts->Map.String.size
      ? Some(
          accountsBalanceRequests->Array.reduce(
            Token.Unit.zero, (acc, balanceRequest) => {
            Token.Unit.Infix.(
              acc
              + balanceRequest
                ->ApiRequest.getDoneOk
                ->Option.getWithDefault(Token.Unit.zero)
            )
          }),
        )
      : None;
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
    let requestState: ApiRequest.requestState(option(PublicKeyHash.t), _) =
      useRequestState(Some((address :> string)));

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
          ->Future.get(_ => resetOperations())
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

  let useRequest = () => {
    let (tokensRequest, _) = useRequestState();
    let apiVersion = useApiVersion();
    tokensRequest->ApiRequest.map(tokens =>
      apiVersion->Option.mapWithDefault(Map.String.empty, v =>
        tokens->Map.String.keep((_, t) =>
          t.TokenRepr.chain == v.Network.chain
        )
      )
    );
  };

  let useGetAll = () => {
    let accountsRequest = useRequest();
    accountsRequest->ApiRequest.getWithDefault(Map.String.empty);
  };

  let useGet = (tokenAddress: option(string)) => {
    let tokens = useGetAll();

    switch (tokenAddress, tokens) {
    | (Some(tokenAddress), tokens) => tokens->Map.String.get(tokenAddress)
    | _ => None
    };
  };

  let useResetAll = () => {
    let (_, setTokensRequest) = useRequestState();
    () => setTokensRequest(ApiRequest.expireCache);
  };

  let useCreate = () => {
    let resetTokens = useResetAll();
    TokensApiRequest.useCreate(~sideEffect=_ => resetTokens(), ());
  };

  let useCheck = () => {
    TokensApiRequest.useCheckTokenContract();
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
    aliases->Map.String.keep((k, _) => !accounts->Map.String.has(k));

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
    ->Option.getWithDefault(Map.String.empty);
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
    accountsRequest->ApiRequest.getWithDefault(Map.String.empty);
  };

  let useGetAllWithDelegates = () => {
    let accounts = useGetAll();
    let delegates = Delegate.useGetAll();

    accounts->Map.String.map(account => {
      let delegate = delegates->Map.String.get((account.address :> string));
      (account, delegate);
    });
  };

  let useGetFromAddress = (address: PublicKeyHash.t) => {
    let accounts = useGetAll();
    accounts->Map.String.get((address :> string));
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

  let useScanGlobal = () => {
    let resetSecrets = useResetAll();

    let requestState = React.useState(() => ApiRequest.NotAsked);
    let (scanRequest, scanGet) =
      SecretApiRequest.useScanGlobal(~requestState, ());

    let scan = input => {
      scanGet(input)->Future.tapOk(_ => resetSecrets());
    };

    (scanRequest, scan);
  };

  let useCreateWithMnemonics = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useCreateWithMnemonics(
      ~sideEffect=_ => resetSecrets(),
      (),
    );
  };

  let useLedgerImport = () => {
    let resetSecrets = useResetAll();
    SecretApiRequest.useLedgerImport(~sideEffect=_ => resetSecrets(), ());
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

    switch (store.selectedAccountState, accounts) {
    | ((Some(selectedAccount), _), accounts) =>
      accounts->Map.String.get((selectedAccount :> string))
    | _ => None
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
    let tokens = Tokens.useGetAll();

    switch (store.selectedTokenState, tokens) {
    | ((Some(selectedToken), _), tokens) =>
      tokens->Map.String.get((selectedToken :> string))
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
      let beaconPeersRequestState = useRequestState();
      BeaconApiRequest.Peers.useLoad(beaconPeersRequestState);
    };

    let useDelete = () => {
      let resetBeaconPeers = useResetAll();
      BeaconApiRequest.Peers.useDelete(
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
      let beaconPermissionsRequestState = useRequestState();
      BeaconApiRequest.Permissions.useLoad(beaconPermissionsRequestState);
    };

    let useDelete = () => {
      let resetBeaconPermissions = useResetAll();
      BeaconApiRequest.Permissions.useDelete(
        ~sideEffect=_ => resetBeaconPermissions(),
        (),
      );
    };
  };
};
