open Belt;
open Common;

type reactState('state) = ('state, ('state => 'state) => unit);
type apiRequestsState('requestResponse) =
  reactState(Map.String.t(ApiRequest.t('requestResponse)));

type state = {
  network: Network.t,
  selectedAccountState: reactState(option(string)),
  selectedTokenState: reactState(option(string)),
  accountsRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
  balanceRequestsState: apiRequestsState(string),
  delegateRequestsState: apiRequestsState(option(string)),
  delegateInfoRequestsState:
    apiRequestsState(DelegateApiRequest.DelegateAPI.delegationInfo),
  operationsRequestsState: apiRequestsState(array(Operation.t)),
  aliasesRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
  bakersRequestState: reactState(ApiRequest.t(array(Delegate.t))),
  tokensRequestState: reactState(ApiRequest.t(Map.String.t(Token.t))),
  balanceTokenRequestsState: apiRequestsState(string),
};

// Context and Provider

let initialApiRequestsState = (Map.String.empty, _ => ());

let initialState = {
  network: Network.Test,
  selectedAccountState: (None, _ => ()),
  selectedTokenState: (None, _ => ()),
  accountsRequestState: (NotAsked, _ => ()),
  balanceRequestsState: initialApiRequestsState,
  delegateRequestsState: initialApiRequestsState,
  delegateInfoRequestsState: initialApiRequestsState,
  operationsRequestsState: initialApiRequestsState,
  aliasesRequestState: (NotAsked, _ => ()),
  bakersRequestState: (NotAsked, _ => ()),
  tokensRequestState: (NotAsked, _ => ()),
  balanceTokenRequestsState: initialApiRequestsState,
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
  let (network, _setNetwork) = React.useState(() => Network.Test);

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

  let aliasesRequestState = React.useState(() => ApiRequest.NotAsked);
  let bakersRequestState = React.useState(() => ApiRequest.NotAsked);
  let tokensRequestState = React.useState(() => ApiRequest.NotAsked);

  AccountApiRequest.useLoad(accountsRequestState)->ignore;
  AliasApiRequest.useLoad(aliasesRequestState)->ignore;

  // Select a default account if no one selected
  React.useEffect2(
    () => {
      if (selectedAccount->Option.isNone) {
        accountsRequest
        ->ApiRequest.getOkWithDefault(Map.String.empty)
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
      network,
      selectedAccountState,
      selectedTokenState,
      accountsRequestState,
      balanceRequestsState,
      delegateRequestsState,
      delegateInfoRequestsState,
      operationsRequestsState,
      aliasesRequestState,
      bakersRequestState,
      tokensRequestState,
      balanceTokenRequestsState,
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
        key->Belt.Option.mapWithDefault(ApiRequest.NotAsked, key =>
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
          setRequests(request =>
            request->Map.String.update(key, oldRequest =>
              Some(newRequestSetter(oldRequest))
            )
          )
        ),
      (key, setRequests),
    );

  (request, setRequest);
};

module Network = {
  let useGet = () => {
    let store = useStoreContext();
    store.network;
  };
};

module Balance = {
  let useRequestState = useRequestsState(store => store.balanceRequestsState);

  let useLoad = (address: string) => {
    let network = Network.useGet();
    let requestState = useRequestState(Some(address));

    BalanceApiRequest.useLoad(~network, ~requestState, ~address);
  };

  let useGetTotal = () => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getOkWithDefault(Map.String.empty);

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
          accountsBalanceRequests
          ->Array.reduce(0.0, (acc, balanceRequest) => {
              acc
              +. balanceRequest
                 ->ApiRequest.getOkWithDefault("0.0")
                 ->Belt.Float.fromString
                 ->Belt.Option.getWithDefault(0.0)
            })
          ->Js.Float.toFixedWithPrecision(~digits=6),
        )
      : None;
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setBalanceRequests) = store.balanceRequestsState;
    () => setBalanceRequests(_ => Map.String.empty);
  };
};

module BalanceToken = {
  let getRequestKey = (address: string, tokenAddress: option(string)) =>
    tokenAddress->Option.map(tokenAddress => address ++ tokenAddress);

  let useRequestState =
    useRequestsState(store => store.balanceTokenRequestsState);

  let useLoad = (address: string, tokenAddress: option(string)) => {
    let network = Network.useGet();
    let requestState = useRequestState(address->getRequestKey(tokenAddress));

    let operation =
      React.useMemo2(
        () =>
          tokenAddress->Belt.Option.map(tokenAddress =>
            Tokens.makeGetBalance(
              address,
              "KT1BZ6cBooBYubKv4Z3kd7izefLXgwTrSfoG",
              tokenAddress,
              (),
            )
          ),
        (address, tokenAddress),
      );

    TokensApiRequest.useLoadOperationOffline(
      ~network,
      ~requestState,
      ~operation,
    );
  };

  let useGetTotal = (tokenAddress: option(string)) => {
    let store = useStoreContext();
    let (balanceRequests, _) = store.balanceTokenRequestsState;
    let (accountsRequest, _) = store.accountsRequestState;
    let accounts =
      accountsRequest->ApiRequest.getOkWithDefault(Map.String.empty);

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
          ->Array.reduce(0.0, (acc, balanceRequest) => {
              acc
              +. balanceRequest
                 ->ApiRequest.getOkWithDefault("0.0")
                 ->Belt.Float.fromString
                 ->Belt.Option.getWithDefault(0.0)
            })
          ->Js.Float.toFixedWithPrecision(~digits=6),
        )
      : None;
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setBalanceTokenRequests) = store.balanceTokenRequestsState;
    () => setBalanceTokenRequests(_ => Map.String.empty);
  };
};

module Delegate = {
  let useRequestState = useRequestsState(store => store.delegateRequestsState);

  let useLoad = (address: string) => {
    let network = Network.useGet();
    let requestState = useRequestState(Some(address));

    DelegateApiRequest.useLoad(~network, ~requestState, ~address);
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
};

module DelegateInfo = {
  let useRequestState =
    useRequestsState(store => store.delegateInfoRequestsState);

  let useLoad = (address: string) => {
    let network = Network.useGet();
    let requestState = useRequestState(Some(address));

    DelegateApiRequest.useLoadInfo(~network, ~requestState, ~address);
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let (_, setDelegateRequests) = store.delegateRequestsState;
    let (_, setDelegateInfoRequests) = store.delegateInfoRequestsState;
    () => {
      setDelegateRequests(_ => Map.String.empty);
      setDelegateInfoRequests(_ => Map.String.empty);
    };
  };
};

module Operations = {
  let useRequestState =
    useRequestsState(store => store.operationsRequestsState);

  let useLoad = (~limit=?, ~types=?, ~address: option(string), ()) => {
    let network = Network.useGet();
    let requestState = useRequestState(address);

    OperationApiRequest.useLoad(
      ~network,
      ~requestState,
      ~limit?,
      ~types?,
      ~address,
      (),
    );
  };

  let useResetAll = () => {
    let store = useStoreContext();
    let resetBalances = Balance.useResetAll();
    let resetBalanceTokens = BalanceToken.useResetAll();
    let resetDelegatesAndDelegatesInfo = DelegateInfo.useResetAll();
    let (_, setOperationsRequests) = store.operationsRequestsState;
    () => {
      setOperationsRequests(_ => Map.String.empty);
      resetBalances();
      resetBalanceTokens();
      resetDelegatesAndDelegatesInfo();
    };
  };

  let useCreate = () => {
    let network = Network.useGet();
    let resetOperations = useResetAll();
    OperationApiRequest.useCreate(
      ~sideEffect=_ => resetOperations(),
      ~network,
    );
  };

  let useSimulate = () => {
    let network = Network.useGet();
    OperationApiRequest.useSimulate(~network);
  };
};

module OperationToken = {
  let useCreate = () => {
    let network = Network.useGet();
    let resetOperations = Operations.useResetAll();
    TokensApiRequest.useCreateOperation(
      ~sideEffect=_ => resetOperations(),
      ~network,
    );
  };

  let useSimulate = () => {
    let network = Network.useGet();
    TokensApiRequest.useSimulate(~network);
  };
};

module Bakers = {
  let useRequestState = () => {
    let store = useStoreContext();
    store.bakersRequestState;
  };

  let useLoad = () => {
    let network = Network.useGet();
    let requestState = useRequestState();

    DelegateApiRequest.useLoadBakers(~network, ~requestState);
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

  let useLoad = () => {
    let requestState = useRequestState();
    TokensApiRequest.useLoadTokens(~requestState);
  };

  let useGetAll = () => {
    let accountsRequest = useRequest();
    accountsRequest->ApiRequest.getOkWithDefault(Map.String.empty);
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
    () => setTokensRequest(_ => NotAsked);
  };

  let useCreate = () => {
    let resetTokens = useResetAll();
    TokensApiRequest.useCreate(~sideEffect=_ => resetTokens(), ());
  };

  let useCheck = () => {
    let network = Network.useGet();
    TokensApiRequest.useCheckTokenContract(~network);
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

  let useResetAll = () => {
    let (_, setAliasesRequest) = useRequestState();
    () => setAliasesRequest(_ => NotAsked);
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
    accountsRequest->ApiRequest.getOkWithDefault(Map.String.empty);
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

  let useResetAll = () => {
    let resetOperations = Operations.useResetAll();
    let resetAliases = Aliases.useResetAll();
    let (_, setAccountsRequest) = useRequestState();
    () => {
      setAccountsRequest(_ => NotAsked);
      resetOperations();
      resetAliases();
    };
  };

  let useCreate = () => {
    let resetAccounts = useResetAll();
    AccountApiRequest.useCreate(~sideEffect=_ => resetAccounts(), ());
  };

  let useCreateWithMnemonics = () => {
    let resetAccounts = useResetAll();
    AccountApiRequest.useCreateWithMnemonics(
      ~sideEffect=_ => resetAccounts(),
      (),
    );
  };

  let useDelete = () => {
    let resetAccounts = useResetAll();
    AccountApiRequest.useDelete(~sideEffect=_ => resetAccounts(), ());
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
