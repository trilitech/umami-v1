open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "main": style(~padding=8.->dp, ~backgroundColor="#ffffff", ()),
      "header":
        style(
          ~flex=1.,
          ~justifyContent=`spaceBetween,
          ~flexDirection=`row,
          (),
        ),
    })
  );

[@react.component]
let make = () => {
  let (network, setNetwork) = React.useState(() => Network.Test);
  let (account, setAccount) =
    React.useState(() => "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3");
  let (injection, setInjection) = React.useState(() => Injection.Done);

  React.useEffect3(
    () => {
      switch (injection) {
      | Pending(transaction) =>
        network
        ->API.Transactions.post(transaction)
        ->FutureEx.getOk(_ => setInjection(_ => Done))
      | Done => ()
      };
      None;
    },
    (network, injection, setInjection),
  );

  <SafeAreaView>
    <Network.Provider value=(network, network => setNetwork(_ => network))>
      <Account.Provider value=(account, account => setAccount(_ => account))>
        <Injection.Provider
          value=(injection, injection => setInjection(_ => injection))>
          <View style=styles##main>
            <View style=styles##header> <Balance /> <NetworkSwitch /> </View>
            <TransactionForm
              onSubmit={
                (amount, destination) =>
                  setInjection(_ =>
                    Pending({source: account, amount, destination})
                  )
              }
            />
            <AccountCreationForm />
            <Transactions />
          </View>
        </Injection.Provider>
      </Account.Provider>
    </Network.Provider>
  </SafeAreaView>;
};