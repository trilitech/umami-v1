type state = {
  network: Network.t,
  selectedAccount: option(string),
  accounts: option(Belt.Map.String.t(Account.t)),
  updateAccount: string => unit,
  operations: array(Operation.t),
  setOperations: (array(Operation.t) => array(Operation.t)) => unit,
};

// Context and Provider

let initialState = {
  network: Network.Test,
  selectedAccount: None,
  accounts: None,
  updateAccount: _ => (),
  operations: [||],
  setOperations: _ => (),
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
  let accountsRequest = AccountApiRequest.useLoad();

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

  let accounts =
    React.useMemo1(
      () => {
        accountsRequest
        ->ApiRequest.getDoneOk
        ->Belt.Option.map(accounts =>
            accounts
            ->Belt.Array.map(((alias, address)) => {
                let account: Account.t = {alias, address};
                (address, account);
              })
            ->Belt.Map.String.fromArray
          )
      },
      [|accountsRequest|],
    );

  let (operations, setOperations) = React.useState(() => [||]);
  let (_addresses, _setAddresses) = React.useState(() => [||]);

  <Provider
    value={
      network,
      selectedAccount,
      accounts,
      updateAccount,
      operations,
      setOperations,
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
  | (Some(selectedAccount), Some(accounts)) =>
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

let useAccountFromAddress = address => {
  let accounts = useAccounts();
  accounts->Belt.Option.flatMap(accounts =>
    accounts->Belt.Map.String.get(address)
  );
};

let getAlias = (accounts, address) => {
  accounts
  ->Belt.Option.flatMap(accounts =>
      accounts
      ->Belt.Map.String.get(address)
      ->Belt.Option.map((acc: Account.t) => acc.alias)
    )
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
