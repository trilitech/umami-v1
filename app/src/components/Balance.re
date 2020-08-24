open ReactNative;

let style = Style.(style(~padding=4.->dp, ()));

[@react.component]
let make = () => {
  let (network, _) = React.useContext(Network.context);
  let (account, _) = React.useContext(Account.context);
  let (injection, _) = React.useContext(Injection.context);

  let (balance, setBalance) = React.useState(() => "");

  React.useEffect4(
    () => {
      switch (injection) {
      | Pending(_) => ()
      | Done =>
        network
        ->API.Balance.get(account)
        ->FutureEx.getOk(value => setBalance(_ => value))
      };
      None;
    },
    (network, account, injection, setBalance),
  );

  <Text style> ("Balance: " ++ balance)->React.string </Text>;
};