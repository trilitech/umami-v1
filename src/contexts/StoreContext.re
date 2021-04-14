open Belt;

open UmamiCommon;

type reactState('state) = ('state, ('state => 'state) => unit);

type requestsState('requestResponse) =
  Map.String.t(ApiRequest.t('requestResponse));

type apiRequestsState('requestResponse) =
  reactState(requestsState('requestResponse));

type state = {
  selectedAccountState: reactState(option(string)),
  selectedTokenState: reactState(option(string)),
  accountsRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
  secretsRequestState: reactState(ApiRequest.t(array(Secret.t))),
  balanceRequestsState: apiRequestsState(ProtocolXTZ.t),
  delegateRequestsState: apiRequestsState(option(string)),
  delegateInfoRequestsState:
    apiRequestsState(option(DelegateApiRequest.DelegateAPI.delegationInfo)),
  operationsRequestsState: apiRequestsState((array(Operation.Read.t), int)),
  operationsConfirmations: reactState(Set.String.t),
  aliasesRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
  bakersRequestState: reactState(ApiRequest.t(array(Delegate.t))),
  tokensRequestState: reactState(ApiRequest.t(Map.String.t(Token.t))),
  balanceTokenRequestsState: apiRequestsState(string),
  apiVersionRequestState: reactState(option(Network.apiVersion)),
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
  let settings = SdkContext.useSettings();
  let addToast = LogsContext.useToast();

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

  let apiVersionRequestState = React.useState(() => None);
  let (apiVersion, setApiVersion) = apiVersionRequestState;

  AccountApiRequest.useLoad(accountsRequestState)->ignore;
  AliasApiRequest.useLoad(aliasesRequestState)->ignore;
  TokensApiRequest.useLoadTokens(tokensRequestState)->ignore;

  React.useEffect0(() => {
    if (apiVersion->Option.isNone) {
      Network.checkConfiguration(
        ~network=settings->AppSettings.network,
        settings->AppSettings.explorer,
        settings->AppSettings.endpoint,
      )
      ->Future.mapOk(apiVersion => {
          if (!Network.checkInBound(apiVersion.Network.api)) {
            addToast(
              Logs.error(
                ~origin=Settings,
                Network.errorMsg(Network.APINotSupported(apiVersion.api)),
              ),
            );
          };
          apiVersion;
        })
      ->FutureEx.getOk(v => setApiVersion(_ => Some(v)));
    };
    None;
  });

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
          setRequests((request: requestsState('requestResponse)) =>
            request->Map.String.update(
              key, (oldRequest: option(ApiRequest.t('requestResponse))) =>
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
  requestsState->Map.String.map(ApiRequest.updateToResetState);

let reloadRequests = requestsState =>
  requestsState->Map.String.map(ApiRequest.updateToLoadingState);

//

module Balance = {
  let useRequestState = useRequestsState(store => store.balanceRequestsState);

  let useLoad = (address: string) => {
    let requestState = useRequestState(Some(address));

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
          balanceRequests->Map.String.get(account.address)
        })
      ->Array.keep(ApiRequest.isDone);

    // check if balance requests for each accounts are done
    accountsBalanceRequests->Array.size == accounts->Map.String.size
      ? Some(
          accountsBalanceRequests->Array.reduce(
            ProtocolXTZ.zero, (acc, balanceRequest) => {
            ProtocolXTZ.Infix.(
              acc
              + balanceRequest
                ->ApiRequest.getDoneOk
                ->Option.getWithDefault(ProtocolXTZ.zero)
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
  let getRequestKey = (address: string, tokenAddress: option(string)) =>
    tokenAddress->Option.map(tokenAddress => address ++ tokenAddress);

  let useRequestState =
    useRequestsState(store => store.balanceTokenRequestsState);

  let useLoad = (address: string, tokenAddress: option(string)) => {
    let requestState = useRequestState(address->getRequestKey(tokenAddress));

    let operation =
      React.useMemo2(
        () =>
          tokenAddress->Option.map(tokenAddress =>
            Token.makeGetBalance(address, tokenAddress, ())
          ),
        (address, tokenAddress),
      );

    TokensApiRequest.useLoadOperationOffline(~requestState, ~operation);
  };

  let useGetTotal = (tokenAddress: option(string)) => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceTokenRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getWithDefault(Map.String.empty);

    let accountsBalanceRequests =
      accounts
      ->Map.String.valuesToArray
      ->Array.keepMap(account => {
          account.address
          ->getRequestKey(tokenAddress)
          ->Option.flatMap(balanceRequests->Map.String.get)
        })
      ->Array.keep(ApiRequest.isDone);

    // check if balance requests for each accounts are done
    accountsBalanceRequests->Array.size == accounts->Map.String.size
      ? Some(
          accountsBalanceRequests
          ->Array.reduce(0, (acc, balanceRequest) => {
              acc
              + balanceRequest
                ->ApiRequest.getDoneOk
                ->Option.flatMap(Belt.Int.fromString)
                ->Option.getWithDefault(0)
            })
          ->Belt.Int.toString,
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

  let useLoad = (address: string) => {
    let requestState: ApiRequest.requestState(option(string)) =
      useRequestState(Some(address));

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

  let useLoad = (address: string) => {
    let requestState = useRequestState(Some(address));

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

  let useLoad = (~limit=?, ~types=?, ~address: option(string), ()) => {
    let requestState = useRequestState(address);

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
    let settings = SdkContext.useSettings();
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

  let useShrinked = filterOut => {
    let requestState = useRequestState();

    DelegateApiRequest.useLoadBakers(~requestState)
    ->ApiRequest.map(a =>
        filterOut
        ->Option.mapWithDefault(a, filterOut =>
            a->Array.keep(v => v.address != filterOut)
          )
        ->Array.shuffle
        ->Array.slice(~offset=0, ~len=5)
      );
  };
};

module Tokens = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.tokensRequestState;
  };

  let useRequest = () => {
    let (tokensRequest, _) = useRequestState();
    tokensRequest;
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
    () => setTokensRequest(ApiRequest.updateToResetState);
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
    () => setAliasesRequest(ApiRequest.updateToResetState);
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
      let delegate = delegates->Map.String.get(account.address);
      (account, delegate);
    });
  };

  let useGetFromAddress = address => {
    let accounts = useGetAll();
    accounts->Map.String.get(address);
  };

  let useResetNames = () => {
    let resetAliases = Aliases.useResetAll();
    let resetOperations = Operations.useResetNames();
    let (_, setAccountsRequest) = useRequestState();
    () => {
      setAccountsRequest(ApiRequest.updateToResetState);
      resetAliases();
      resetOperations();
    };
  };

  let useResetAll = () => {
    let resetOperations = Operations.useResetAll();
    let resetAliases = Aliases.useResetAll();
    let (_, setAccountsRequest) = useRequestState();
    () => {
      setAccountsRequest(ApiRequest.updateToResetState);
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
      setSecretsRequest(ApiRequest.updateToResetState);
    };
  };

  let useResetAll = () => {
    let resetAccounts = Accounts.useResetAll();
    let (_, setSecretsRequest) = useRequestState();
    () => {
      setSecretsRequest(ApiRequest.updateToResetState);
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
      accounts->Map.String.get(selectedAccount)
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
      tokens->Map.String.get(selectedToken)
    | _ => None
    };
  };

  let useSet = () => {
    let store = useStoreContext();
    let (_, setSelectedToken) = store.selectedTokenState;

    newToken => setSelectedToken(_ => newToken);
  };
};
