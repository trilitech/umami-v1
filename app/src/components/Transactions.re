open ReactNative;

let style = Style.(style(~padding=4.->dp, ()));

[@react.component]
let make = () => {
  let (network, _) = React.useContext(Network.context);
  let (account, _) = React.useContext(Account.context);
  let (injection, _) = React.useContext(Injection.context);

  let (transactions: array(Operation.transaction), setTransactions) =
    React.useState(() => [||]);

  React.useEffect4(
    () => {
      switch (injection) {
      | Pending(_) => ()
      | Done =>
        network
        ->API.Transactions.get(account)
        ->FutureEx.getOk(value => setTransactions(_ => value))
      };
      None;
    },
    (network, account, injection, setTransactions),
  );

  <FlatList
    data=transactions
    keyExtractor={(transaction, _) => transaction.id}
    renderItem={
      transaction =>
        <View style>
          <Text>
            ("Date: " ++ Js.Date.toLocaleString(transaction.item.time))
            ->React.string
          </Text>
          <Text> ("Sender: " ++ transaction.item.sender)->React.string </Text>
          <Text>
            ("Receiver: " ++ transaction.item.receiver)->React.string
          </Text>
          <Text>
            (
              "Amount: "
              ++ Js.Float.toString(transaction.item.amount)
              ++ {js| ꜩ|js}
            )
            ->React.string
          </Text>
          <Text>
            ("Fee: " ++ transaction.item.fee ++ {js| μꜩ|js})->React.string
          </Text>
        </View>
    }
  />;
};