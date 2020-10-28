open Belt;
open ReactNative;

module AccountsAPI = API.Accounts(API.TezosClient);
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);
module AliasesAPI = API.Aliases(API.TezosClient);

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

let dummy: array((string, string)) = [||];

let toString = array =>
  array->Array.reduce("", (result, (name, address)) =>
    result
    ++ (result->String.length == 0 ? "" : "\n")
    ++ name
    ++ ": "
    ++ address
  );

[@react.component]
let make = () => {
  let (network, setNetwork) = React.useState(() => Network.Test);
  let (account, setAccount) =
    React.useState(() => "tz1QHESqw4VduUUyGEY9gLh5STBDuTacpydB");
  let (balance, setBalance) = React.useState(() => "");
  let (injection, setInjection) = React.useState(() => InjectionState.Done);
  let (accounts, setAccounts) = React.useState(() => dummy);

  React.useEffect0(() => {
    /*
         SecureStorage.Cipher.encrypt("yo", "ga")
         ->FutureEx.getOk(SecureStorage.setEncryptedData("test"));

         switch (SecureStorage.getEncryptedData("test")) {
         | Some(data) =>
           data
           ->SecureStorage.Cipher.decrypt("sdf")
           ->Future.tapError(Js.log)
           ->FutureEx.getOk(Js.log)
         | None => ()
         };

         SecureStorage.clear();

         SecureStorage.getEncryptedData("test")->Js.log;
     */
    /*
    Network.Test
    ->OperationsAPI.simulate(
        Transaction(
          Injection.makeTransfer(
            ~source="bob",
            ~amount=1,
            ~destination="zebra",
            (),
          ),
        ),
      )
    ->Future.tapError(Js.log)
    ->FutureEx.getOk(Js.log);
    */
    None;
  });

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
      AccountsAPI.get()
      ->Future.tapOk(value => setAccounts(_ => value))
      ->ignore;
      None;
    },
    [|setAccounts|],
  );

  <NetworkState.Provider value=(network, network => setNetwork(_ => network))>
    <AccountState.Provider
      value=(account, account => setAccount(_ => account))>
      <BalanceState.Provider
        value=(balance, balance => setBalance(_ => balance))>
        <AccountsState.Provider
          value=(accounts, accounts => setAccounts(_ => accounts))>
          <InjectionState.Provider
            value=(injection, injection => setInjection(_ => injection))>
            <View style=styles##main>
              <View style=styles##header>
                <BalanceOverview />
                <NetworkSwitch />
              </View>
              <TransactionForm
                onSubmit={(source, amount, destination) =>
                  setInjection(_ =>
                    Pending(
                      Transaction(
                        Injection.makeTransfer(
                          ~source,
                          ~amount,
                          ~destination,
                          (),
                        ),
                      ),
                    )
                  )
                }
              />
              <DelegateForm
                onSubmit={(source, delegate) =>
                  AccountsAPI.add("delegate", delegate)
                  ->Future.tapOk(_ =>
                      AccountsAPI.get()
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
              <AccountHDRestorationForm />
              <AccountDeletionForm />
              <Text style=styles##section>
                {accounts->toString->React.string}
              </Text>
              <OperationList />
            </View>
          </InjectionState.Provider>
        </AccountsState.Provider>
      </BalanceState.Provider>
    </AccountState.Provider>
  </NetworkState.Provider>;
};
