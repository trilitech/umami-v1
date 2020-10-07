open ReactNative;

module MapString = Belt.Map.String;

let styles =
  Style.(
    StyleSheet.create({
      "main": style(~padding=8.->dp, ~backgroundColor="#ffffff", ()),
      "header":
        style(
          ~flex=1.,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          ~margin=4.->dp,
          ~borderWidth=1.0,
          (),
        ),
      "section": style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()),
    })
  );

let dummy: MapString.t(string) = MapString.empty;

let toString = map =>
  map->MapString.reduce("", (result, key, value) =>
    result ++ (result->String.length == 0 ? "" : "\n") ++ key ++ ": " ++ value
  );

module Operations = API.Operations(API.TezosExplorer);

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
        ->Operations.create(operation)
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
      API.Accounts.get()->FutureEx.getOk(value => setAccounts(_ => value));
      None;
    },
    [|setAccounts|],
  );

  <SafeAreaView>
    <Network.Provider value=(network, network => setNetwork(_ => network))>
      <Account.Provider value=(account, account => setAccount(_ => account))>
        <Balance.Provider
          value=(balance, balance => setBalance(_ => balance))>
          <Accounts.Provider
            value=(accounts, accounts => setAccounts(_ => accounts))>
            <Injection.Provider
              value=(injection, injection => setInjection(_ => injection))>
              <View style=styles##main>
                <View style=styles##header>
                  <BalanceOverview />
                  <NetworkSwitch />
                </View>
                <TransactionForm
                  onSubmit={(source, amount, destination) =>
                    setInjection(_ =>
                      Pending(Transaction({source, amount, destination}))
                    )
                  }
                />
                <DelegateForm
                  onSubmit={(source, delegate) =>
                    API.Accounts.add("delegate", delegate)
                    ->Future.tapOk(_ =>
                        API.Accounts.get()
                        ->FutureEx.getOk(value => setAccounts(_ => value))
                      )
                    ->FutureEx.getOk(_ =>
                        setInjection(_ =>
                          Pending(Delegation({source, delegate: "delegate"}))
                        )
                      )
                  }
                />
                <AccountCreationForm />
                <AccountRestorationForm />
                <AccountDeletionForm />
                <Text style=styles##section>
                  {accounts->toString->React.string}
                </Text>
                <OperationList />
              </View>
            </Injection.Provider>
          </Accounts.Provider>
        </Balance.Provider>
      </Account.Provider>
    </Network.Provider>
  </SafeAreaView>;
};
