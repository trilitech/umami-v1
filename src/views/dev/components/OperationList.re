open ReactNative;

let style = Style.(style(~padding=4.->dp, ()));

let formated_amount = (transaction: Operation.Business.Transaction.t) =>
  transaction.amount
  ->Js.Float.fromString
  ->(x => x /. 1000000.)
  ->Js.Float.toString
  ++ {js| ꜩ|js};

module BalanceAPI = API.Balance(API.TezosClient);
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let (network, _) = React.useContext(NetworkState.context);
  let (account, _) = React.useContext(AccountState.context);
  let (_, setBalance) = React.useContext(BalanceState.context);
  let (injection, _) = React.useContext(InjectionState.context);
  let config = ConfigContext.useConfig();

  let (operations: array(Operation.t), setOperations) =
    React.useState(() => [||]);

  React.useEffect4(
    () => {
      switch (injection) {
      | Pending(_) => ()
      | Done =>
        (network, config)
        ->OperationsAPI.get(account, ())
        ->FutureEx.getOk(value => setOperations(_ => value))
      };
      None;
    },
    (network, account, injection, setOperations),
  );

  <View>
    <Button
      onPress={_ => {
        (network, config)
        ->BalanceAPI.get(account)
        ->FutureEx.getOk(value => setBalance(value));
        (network, config)
        ->OperationsAPI.get(account, ())
        ->FutureEx.getOk(value => setOperations(_ => value));
      }}
      title="Refresh"
    />
    <FlatList
      data=operations
      keyExtractor={(operation, index) =>
        operation.id ++ ":" ++ index->string_of_int
      }
      renderItem={operation =>
        <View style>
          <Text>
            {("Date: " ++ Js.Date.toLocaleString(operation.item.timestamp))
             ->React.string}
          </Text>
          {switch (operation.item.payload) {
           | Business(payload) =>
             <View>
               {switch (payload.payload) {
                | Reveal(reveal) =>
                  <Text>
                    {("Reveal public key " ++ reveal.public_key)->React.string}
                  </Text>
                | Transaction(transaction) =>
                  <Text>
                    (
                      if (transaction.amount == "0") {
                        "Call contract " ++ transaction.destination;
                      } else if (payload.source == transaction.destination) {
                        transaction->formated_amount ++ " to itself";
                      } else if (payload.source == account) {
                        "-"
                        ++ transaction->formated_amount
                        ++ " to "
                        ++ transaction.destination;
                      } else {
                        "+"
                        ++ transaction->formated_amount
                        ++ " from "
                        ++ payload.source;
                      }
                    )
                    ->React.string
                  </Text>
                | Origination(origination) =>
                  <Text>
                    {("New contract " ++ origination.contract_address)
                     ->React.string}
                  </Text>
                | Delegation(delegation) =>
                  switch (delegation.delegate) {
                  | Some(delegate) =>
                    <Text>
                      (
                        if (payload.source == delegate) {
                          I18n.t#operation_menu_register_as_baker;
                        } else {
                          I18n.t#operation_menu_delegate_to(delegate);
                        }
                      )
                      ->React.string
                    </Text>
                  | None =>
                    <Text>
                      I18n.t#operation_menu_cancel_delegation->React.string
                    </Text>
                  }
                | Unknown =>
                  <Text> I18n.t#operation_menu_unknown->React.string </Text>
                }}
               <Text>
                 {("fee " ++ payload.fee ++ {js| μꜩ|js})->React.string}
               </Text>
             </View>
           }}
        </View>
      }
    />
  </View>;
};
