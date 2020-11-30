open Belt;
open Common;

type reactState('state) = ('state, ('state => 'state) => unit);
type apiRequestsState('requestResponse) =
  reactState(Belt.Map.String.t(ApiRequest.t('requestResponse)));

type state = {
  network: Network.t,
  selectedAccount: option(string),
  refreshAccounts: (~loading: bool=?, unit) => unit,
  accountsRequest: ApiRequest.t(array((string, string))),
  balanceRequestsState: apiRequestsState(string),
  delegateRequestsState: apiRequestsState(option(string)),
  updateAccount: string => unit,
  operations: array(Operation.t),
  setOperations: (array(Operation.t) => array(Operation.t)) => unit,
  aliases: array((string, string)),
  setAliases: (array((string, string)) => array((string, string))) => unit,
};

// Context and Provider

let initialApiRequestsState = (Belt.Map.String.empty, _ => ());

let initialState = {
  network: Network.Test,
  selectedAccount: None,
  refreshAccounts: (~loading as _=?, ()) => (),
  accountsRequest: NotAsked,
  balanceRequestsState: initialApiRequestsState,
  delegateRequestsState: initialApiRequestsState,
  updateAccount: _ => (),
  operations: [||],
  setOperations: _ => (),
  aliases: [||],
  setAliases: _ => (),
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
  let (selectedAccount, setSelectedAccount) = React.useState(() => None);

  let updateAccount = newAccount => setSelectedAccount(_ => Some(newAccount));
  let (getAccounts, accountsRequest) = AccountApiRequest.useGet();

  let balanceRequestsState = React.useState(() => Belt.Map.String.empty);
  let delegateRequestsState = React.useState(() => Belt.Map.String.empty);

  React.useEffect0(() => {
    getAccounts()->ignore;
    None;
  });

  let refreshAccounts = (~loading=?, ()) =>
    getAccounts(~loading?, ())->ignore;

  React.useEffect2(
    () => {
      if (selectedAccount->Belt.Option.isNone) {
        accountsRequest
        ->ApiRequest.getOkWithDefault([||])
        ->Belt.Array.reverse
        ->Belt.Array.get(0)
        ->Lib.Option.iter(((_alias, address)) =>
            setSelectedAccount(_ => Some(address))
          );
      };
      None;
    },
    (accountsRequest, selectedAccount),
  );

  let (operations, setOperations) = React.useState(() => [||]);
  let (aliases, setAliases) = React.useState(() => [||]);

  <Provider
    value={
      network,
      selectedAccount,
      refreshAccounts,
      accountsRequest,
      balanceRequestsState,
      delegateRequestsState,
      updateAccount,
      operations,
      setOperations,
      aliases,
      setAliases,
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

let useAccountsRequest = () => {
  let store = useStoreContext();
  store.accountsRequest;
};

let useAccounts = () => {
  let accountsRequest = useAccountsRequest();

  accountsRequest
  ->ApiRequest.getOkWithDefault([||])
  ->Belt.Array.map(((alias, address)) => {
      let account: Account.t = {alias, address};
      (address, account);
    })
  ->Belt.Array.reverse
  ->Belt.Map.String.fromArray;
};

let useAccount = () => {
  let store = useStoreContext();
  let accounts = useAccounts();

  switch (store.selectedAccount, accounts) {
  | (Some(selectedAccount), accounts) =>
    accounts->Belt.Map.String.get(selectedAccount)
  | _ => None
  };
};

let useUpdateAccount = () => {
  let store = useStoreContext();
  store.updateAccount;
};

let useRefreshAccounts = () => {
  let store = useStoreContext();
  store.refreshAccounts;
};

let useAccountFromAddress = address => {
  let accounts = useAccounts();
  accounts->Belt.Map.String.get(address);
};

// Utils

let useRequestsState = (getRequestsState, key) => {
  let store = useStoreContext();
  let (requests, setRequests) = store->getRequestsState;

  let request =
    React.useMemo1(
      () =>
        requests
        ->Map.String.get(key)
        ->Option.getWithDefault(ApiRequest.NotAsked),
      [|requests|],
    );

  let setRequest =
    React.useCallback1(
      newRequest =>
        setRequests(request =>
          request->Belt.Map.String.update(key, _ => Some(newRequest))
        ),
      [|setRequests|],
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

  accounts->Belt.Map.String.map(account => {
    let delegate = delegates->Belt.Map.String.get(account.address);
    (account, delegate);
  });
};

let getAlias = (accounts, address) => {
  accounts
  ->Belt.Map.String.get(address)
  ->Belt.Option.map((acc: Account.t) => acc.alias)
  ->Belt.Option.getWithDefault(address);
};

let useSetOperations = () => {
  let store = useStoreContext();
  store.setOperations;
};

let useOperations = () => {
  let store = useStoreContext();
  store.operations;
};

let useSetAliases = () => {
  let store = useStoreContext();
  store.setAliases;
};

let useAliases = () => {
  let store = useStoreContext();
  store.aliases;
};
