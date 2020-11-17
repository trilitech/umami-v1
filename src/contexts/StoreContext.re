type state = {
  network: Network.t,
  selectedAccount: option(string),
  accounts: Belt.Map.String.t(Account.t),
  refreshAccounts: (~loading: bool=?, unit) => unit,
  accountsRequest: ApiRequest.t(array((string, string))),
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

  React.useEffect3(
    () => {
      if (selectedAccount->Belt.Option.isNone) {
        let firstAccount =
          accountsRequest
          ->ApiRequest.getDoneOk
          ->Belt.Option.getWithDefault([||])
          ->Belt.Array.reverse
          ->Belt.Array.get(0);

        switch (firstAccount) {
        | Some((_alias, address)) => setSelectedAccount(_ => Some(address))
        | None => ()
        };
      };
      None;
    },
    (accountsRequest, selectedAccount, setSelectedAccount),
  );

  let refreshAccounts = (~loading=?, ()) =>
    getAccounts(~loading?, ())->ignore;

  let accounts =
    React.useMemo1(
      () => {
        accountsRequest
        ->ApiRequest.getOkWithDefault([||])
        ->Belt.Array.map(((alias, address)) => {
            let account: Account.t = {alias, address};
            (address, account);
          })
        ->Belt.Map.String.fromArray
      },
      [|accountsRequest|],
    );

  let (operations, setOperations) = React.useState(() => [||]);
  let (aliases, setAliases) = React.useState(() => [||]);

  <Provider
    value={
      network,
      selectedAccount,
      accounts,
      refreshAccounts,
      accountsRequest,
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
