open Belt;
open Common;

type reactState('state) = ('state, ('state => 'state) => unit);
type apiRequestsState('requestResponse) =
  reactState(Map.String.t(ApiRequest.t('requestResponse)));

type state = {
  network: Network.t,
  selectedAccountState: reactState(option(string)),
  accountsRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
  balanceRequestsState: apiRequestsState(string),
  delegateRequestsState: apiRequestsState(option(string)),
  operationsRequestsState: apiRequestsState(array(Operation.t)),
  aliasesRequestState: reactState(ApiRequest.t(Map.String.t(Account.t))),
};

// Context and Provider

let initialApiRequestsState = (Map.String.empty, _ => ());

let initialState = {
  network: Network.Test,
  selectedAccountState: (None, _ => ()),
  accountsRequestState: (NotAsked, _ => ()),
  balanceRequestsState: initialApiRequestsState,
  delegateRequestsState: initialApiRequestsState,
  operationsRequestsState: initialApiRequestsState,
  aliasesRequestState: (NotAsked, _ => ()),
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

  let accountsRequestState = React.useState(() => ApiRequest.NotAsked);
  let (accountsRequest, _setAccountsRequest) = accountsRequestState;

  let balanceRequestsState = React.useState(() => Map.String.empty);
  let delegateRequestsState = React.useState(() => Map.String.empty);
  let operationsRequestsState = React.useState(() => Map.String.empty);

  let aliasesRequestState = React.useState(() => ApiRequest.NotAsked);

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
      accountsRequestState,
      balanceRequestsState,
      delegateRequestsState,
      operationsRequestsState,
      aliasesRequestState,
    }>
    children
  </Provider>;
};

// Hooks

let useStoreContext = () => React.useContext(context);

let useNetwork = () => {
  let store = useStoreContext();
  store.network;
};

// Account

let useAccountsRequestState = () => {
  let store = useStoreContext();
  store.accountsRequestState;
};

let useAccountsRequest = () => {
  let (accountsRequest, _) = useAccountsRequestState();
  accountsRequest;
};

let useAccounts = () => {
  let accountsRequest = useAccountsRequest();
  accountsRequest->ApiRequest.getOkWithDefault(Map.String.empty);
};

let useAccount = () => {
  let store = useStoreContext();
  let accounts = useAccounts();

  switch (store.selectedAccountState, accounts) {
  | ((Some(selectedAccount), _), accounts) =>
    accounts->Map.String.get(selectedAccount)
  | _ => None
  };
};

let useUpdateAccount = () => {
  let store = useStoreContext();
  let (_, setSelectedAccount) = store.selectedAccountState;

  newAccount => setSelectedAccount(_ => Some(newAccount));
};

let useAccountFromAddress = address => {
  let accounts = useAccounts();
  accounts->Map.String.get(address);
};

let useResetAccounts = () => {
  let (_, setAccountsRequest) = useAccountsRequestState();
  () => setAccountsRequest(_ => NotAsked);
};

let useCreateAccount = () => {
  let resetAccounts = useResetAccounts();
  AccountApiRequest.useCreate(~sideEffect=_ => resetAccounts(), ());
};

let useCreateAccountWithMnemonics = () => {
  let resetAccounts = useResetAccounts();
  AccountApiRequest.useCreateWithMnemonics(
    ~sideEffect=_ => resetAccounts(),
    (),
  );
};

let useDeleteAccount = () => {
  let resetAccounts = useResetAccounts();
  AccountApiRequest.useDelete(~sideEffect=_ => resetAccounts(), ());
};

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

// Balance

let useBalanceRequestState =
  useRequestsState(store => store.balanceRequestsState);

// Delegates

let useDelegateRequestState =
  useRequestsState(store => store.delegateRequestsState);

let useDelegates = () => {
  let store = useStoreContext();
  let (delegateRequests, _) = store.delegateRequestsState;

  delegateRequests
  ->Map.String.map(request =>
      request->ApiRequest.getDoneOk->Option.flatMap(v => v)
    )
  ->Map.String.keep((_k, v) => v->Option.isSome)
  ->Map.String.map(Option.getExn);
};

let useAccountsWithDelegates = () => {
  let accounts = useAccounts();
  let delegates = useDelegates();

  accounts->Map.String.map(account => {
    let delegate = delegates->Map.String.get(account.address);
    (account, delegate);
  });
};

// Operations

let useOperationsRequestState =
  useRequestsState(store => store.operationsRequestsState);

let useResetOperations = () => {
  let store = useStoreContext();
  let (_, setOperationsRequests) = store.operationsRequestsState;
  () => setOperationsRequests(_ => Map.String.empty);
};

// Alias

let useAliasesRequestState = () => {
  let store = useStoreContext();
  store.aliasesRequestState;
};

let useAliasesRequest = () => {
  let (aliasesRequest, _) = useAliasesRequestState();
  aliasesRequest;
};

let useResetAliases = () => {
  let (_, setAliasesRequest) = useAliasesRequestState();
  () => setAliasesRequest(_ => NotAsked);
};

let useAliases = () => {
  let aliasesRequest = useAliasesRequest();
  aliasesRequest
  ->ApiRequest.getDoneOk
  ->Option.getWithDefault(Map.String.empty);
};

let useCreateAlias = () => {
  let resetAliases = useResetAliases();
  AliasApiRequest.useCreate(~sideEffect=_ => resetAliases(), ());
};

let useDeleteAlias = () => {
  let resetAliases = useResetAliases();
  AliasApiRequest.useDelete(~sideEffect=_ => resetAliases(), ());
};
