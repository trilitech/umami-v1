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

[@react.component]
let make = () => {
  let (network, setNetwork) = React.useState(() => Network.Test);
  let (account, setAccount) =
    React.useState(() => "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3");
  let (injection, setInjection) = React.useState(() => Injection.Done);
  let (accounts, setAccounts) = React.useState(() => dummy);

  React.useEffect3(
    () => {
      switch (injection) {
      | Pending(transaction) =>
        network
        ->API.Transactions.create(transaction)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => setInjection(_ => Done)
            | Error(value) => Dialog.error(value)
            }
          )
      | Done => ()
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
        <Accounts.Provider
          value=(accounts, accounts => setAccounts(_ => accounts))>
          <Injection.Provider
            value=(injection, injection => setInjection(_ => injection))>
            <View style=styles##main>
              <View style=styles##header> <Balance /> <NetworkSwitch /> </View>
              <TransactionForm
                onSubmit={(source, amount, destination) =>
                  setInjection(_ => Pending({source, amount, destination}))
                }
              />
              <AccountCreationForm />
              <AccountRestorationForm />
              <AccountDeletionForm />
              <Text style=styles##section>
                {accounts->toString->React.string}
              </Text>
              <Operations />
            </View>
          </Injection.Provider>
        </Accounts.Provider>
      </Account.Provider>
    </Network.Provider>
  </SafeAreaView>;
};
