open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

[@react.component]
let make = () => {
  let config = ConfigContext.useConfig();
  let (source, setSource) = React.useState(() => "");
  let (amount1, setAmount1) = React.useState(() => 0.0);
  let (destination1, setDestination1) = React.useState(() => "");
  let (amount2, setAmount2) = React.useState(() => 0.0);
  let (destination2, setDestination2) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setSource(_ => text)}
      placeholder="source"
      value=source
    />
    <TextInput
      onChangeText={text =>
        text
        ->Js.Float.fromString
        ->(x => Js.Float.isNaN(x) ? amount1 : x)
        ->(x => setAmount1(_ => x))
      }
      placeholder="amount1"
      value={amount1 == 0.0 ? "" : Js.Float.toString(amount1)}
    />
    <TextInput
      onChangeText={text => setDestination1(_ => text)}
      placeholder="destination1"
      value=destination1
    />
    <TextInput
      onChangeText={text =>
        text
        ->Js.Float.fromString
        ->(x => Js.Float.isNaN(x) ? amount2 : x)
        ->(x => setAmount2(_ => x))
      }
      placeholder="amount2"
      value={amount2 == 0.0 ? "" : Js.Float.toString(amount2)}
    />
    <TextInput
      onChangeText={text => setDestination2(_ => text)}
      placeholder="destination2"
      value=destination2
    />
    <Button
      onPress={_ => {
        [@warning "-8"]
        let tx1 =
          Protocol.makeTransfer(
            ~amount=amount1,
            ~destination=destination1,
            (),
          );
        [@warning "-8"]
        let tx2 =
          Protocol.makeTransfer(
            ~amount=amount2,
            ~destination=destination2,
            (),
          );
        Protocol.makeTransaction(~source, ~transfers=[tx1, tx2], ())
        ->(
            txs => {
              [@warning "-8"]
              let Protocol.Transaction(btxs) = txs;
              Js.log(OperationsAPI.transfers_to_json(btxs));
              txs;
            }
          )
        ->OperationsAPI.simulate((Network.Test, config), _)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => Dialog.error("ok")
            | Error(err) => Dialog.error(err)
            }
          );
      }}
      title="Send"
    />
  </View>;
};
