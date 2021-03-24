open ReactNative;

module BalanceAPI = API.Balance;
module AccountsAPI = API.Accounts(API.TezosClient, API.TezosExplorer);
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);
module AliasesAPI = API.Aliases(API.TezosClient);
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

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
  let (network, setNetwork) = React.useState(() => AppSettings.Testnet);
  let (account, setAccount) =
    React.useState(() => "tz1QHESqw4VduUUyGEY9gLh5STBDuTacpydB");
  let (balance, setBalance) = React.useState(() => ProtocolXTZ.zero);
  let (injection, setInjection) = React.useState(() => InjectionState.Done);
  let (accounts, setAccounts) = React.useState(() => dummy);
  let settings = SdkContext.useSettings();

  React.useEffect0(() => {
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
    /*
     (network, config)
     ->DelegateAPI.getDelegationInfoForAccount(
         "tz1RDrPJYrpdA7MbfUJSamGdcfceLXGpUdc7",
       )
     ->Future.tapError(Js.log)
     ->FutureEx.getOk(Js.log);
     */
    AppSettings.withNetwork(settings, network)
    ->OperationsAPI.waitForOperationConfirmations(
        "oo38L1jPWGmf1RBg8zyq4BoUys9zhofWJN8e8eQPnB4D6NQctv5",
        ~confirmations=0,
        ~branch="BMNnzkSH6oHZpfroJPczV9DcASFRsHAQ3qGD7mzB9nQ6EmGGM7n",
      )
    ->Future.tapError(Js.log)
    ->FutureEx.getOk(Js.log);
    None;
  });

  React.useEffect3(
    () => {
      switch (injection) {
      | Pending(operation) =>
        AppSettings.withNetwork(settings, network)
        ->OperationsAPI.inject(operation, ~password="blerot")
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
      AccountsAPI.get(~settings)
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
                      Protocol.makeSingleTransaction(
                        ~source,
                        ~amount,
                        ~destination,
                        (),
                      )
                      ->Protocol.transfer,
                    )
                  )
                }
              />
              <DelegateFormView
                onSubmit={(source, delegate) =>
                  AccountsAPI.add(~settings, "delegate", delegate)
                  ->Future.tapOk(_ =>
                      AccountsAPI.get(~settings)
                      ->FutureEx.getOk(value => setAccounts(_ => value))
                    )
                  ->FutureEx.getOk(_ =>
                      setInjection(_ =>
                        Pending(
                          Protocol.makeDelegate(
                            ~source,
                            ~delegate=Some("delegate"),
                            (),
                          )
                          ->Delegation,
                        )
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
              <TokensCheckForm />
              <TokensGetBalanceForm />
              <TokensTransferForm />
              <BatchTransactionsForm />
              <BatchTokensTxsForm />
              <OperationList />
            </View>
          </InjectionState.Provider>
        </AccountsState.Provider>
      </BalanceState.Provider>
    </AccountState.Provider>
  </NetworkState.Provider>;
};
