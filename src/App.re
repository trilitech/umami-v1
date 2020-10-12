open Belt;

let dummy: Map.String.t(string) = Map.String.empty;

module AccountsAPI = API.Accounts(API.TezosClient);
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let (network, setNetwork) = React.useState(() => Network.Test);
  let (account, setAccount) =
    React.useState(() => "tz1QHESqw4VduUUyGEY9gLh5STBDuTacpydB");
  let (balance, setBalance) = React.useState(() => "");
  let (injection, setInjection) = React.useState(() => Injection.Done);
  let (accounts, setAccounts) = React.useState(() => dummy);

  React.useEffect3(
    () => {
      switch (injection) {
      | Pending(operation) =>
        network
        ->OperationsAPI.create(operation)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => setInjection(_ => Done)
            | Error(value) => Dialog.error(value)
            }
          )
      | Done =>
        Js.log("operation injected");
        ();
      };
      None;
    },
    (network, injection, setInjection),
  );

  React.useEffect1(
    () => {
      AccountsAPI.get()->FutureEx.getOk(value => setAccounts(_ => value));
      None;
    },
    [|setAccounts|],
  );

  <Network.Provider value=(network, network => setNetwork(_ => network))>
    <Account.Provider value=(account, account => setAccount(_ => account))>
      <Balance.Provider value=(balance, balance => setBalance(_ => balance))>
        <Accounts.Provider
          value=(accounts, accounts => setAccounts(_ => accounts))>
          <Injection.Provider
            value=(injection, injection => setInjection(_ => injection))>
            <DevView setInjection setAccounts accounts />
          </Injection.Provider>
        </Accounts.Provider>
      </Balance.Provider>
    </Account.Provider>
  </Network.Provider>;
};
