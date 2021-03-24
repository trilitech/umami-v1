open ReactNative;

module BalanceAPI = API.Balance;

let style = Style.(style(~padding=4.->dp, ()));

[@react.component]
let make = () => {
  let (network, _) = React.useContext(NetworkState.context);
  let (account, _) = React.useContext(AccountState.context);
  let (injection, _) = React.useContext(InjectionState.context);

  let (balance, setBalance) = React.useContext(BalanceState.context);
  let settings = SdkContext.useSettings();

  React.useEffect5(
    () => {
      switch (injection) {
      | Pending(_) => ()
      | Done =>
        settings
        ->BalanceAPI.get(account, ())
        ->FutureEx.getOk(value => setBalance(value))
      };
      None;
    },
    (network, account, injection, balance, setBalance),
  );

  <Text style>
    {("Balance: " ++ balance->ProtocolXTZ.toString)->React.string}
  </Text>;
};
