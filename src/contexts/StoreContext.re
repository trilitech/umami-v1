open Common;

type state = {
  network: Network.t,
  selectedAccount: option(string),
  accounts: Belt.Map.String.t(Account.t),
  refreshAccounts: (~loading: bool=?, unit) => unit,
  accountsRequest: ApiRequest.t(array((string, string))),
  delegates: Belt.Map.String.t(string),
  setDelegates:
    (Belt.Map.String.t(string) => Belt.Map.String.t(string)) => unit,
  updateAccount: string => unit,
  operations: array(Operation.t),
  setOperations: (array(Operation.t) => array(Operation.t)) => unit,
  aliases: array((string, string)),
  setAliases: (array((string, string)) => array((string, string))) => unit,
};

// Context and Provider

let initialState = {
  network: Network.Test,
  selectedAccount: None,
  accounts: Belt.Map.String.empty,
  refreshAccounts: (~loading as _=?, ()) => (),
  accountsRequest: NotAsked,
  delegates: Belt.Map.String.empty,
  setDelegates: _ => (),
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

  React.useEffect0(() => {
    getAccounts()->ignore;
    None;
  });

  let refreshAccounts = (~loading=?, ()) =>
    getAccounts(~loading?, ())->ignore;

  let accountsArray =
    React.useMemo1(
      () => {
        accountsRequest
        ->ApiRequest.getOkWithDefault([||])
        ->Belt.Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Belt.Array.reverse
      },
      [|accountsRequest|],
    );
  let accounts =
    React.useMemo1(
      () => {accountsArray->Belt.Map.String.fromArray},
      [|accountsArray|],
    );

  React.useEffect2(
    () => {
      if (selectedAccount->Belt.Option.isNone) {
        accountsArray
        ->Belt.Array.get(0)
        ->Lib.Option.iter(((address, _)) =>
            setSelectedAccount(_ => Some(address))
          );
      };
      None;
    },
    (accounts, selectedAccount),
  );

  let (delegates, setDelegates) = React.useState(() => Belt.Map.String.empty);
  let (operations, setOperations) = React.useState(() => [||]);
  let (aliases, setAliases) = React.useState(() => [||]);

  <Provider
    value={
      network,
      selectedAccount,
      accounts,
      refreshAccounts,
      accountsRequest,
      delegates,
      setDelegates,
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

let useAccount = () => {
  let store = useStoreContext();

  switch (store.selectedAccount, store.accounts) {
  | (Some(selectedAccount), accounts) =>
    accounts->Belt.Map.String.get(selectedAccount)
  | _ => None
  };
};

let useUpdateAccount = () => {
  let store = useStoreContext();
  store.updateAccount;
};

let useAccounts = () => {
  let store = useStoreContext();
  store.accounts;
};

let useRefreshAccounts = () => {
  let store = useStoreContext();
  store.refreshAccounts;
};

let useAccountsRequest = () => {
  let store = useStoreContext();
  store.accountsRequest;
};

let useAccountFromAddress = address => {
  let accounts = useAccounts();
  accounts->Belt.Map.String.get(address);
};

let useDelegates = () => {
  let store = useStoreContext();
  store.delegates;
};

let useAccountDelegate = address => {
  let delegates = useDelegates();
  delegates->Belt.Map.String.get(address);
};

let useSetAccountDelegate = () => {
  let store = useStoreContext();
  (address, delegate) => {
    delegate->Common.Lib.Option.iter(delegate =>
      store.setDelegates(delegates =>
        delegates->Belt.Map.String.set(address, delegate)
      )
    );
  };
};

let useAccountsWithDelegates = () => {
  let store = useStoreContext();
  store.accounts
  ->Belt.Map.String.map(account => {
      let delegate = store.delegates->Belt.Map.String.get(account.address);
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
