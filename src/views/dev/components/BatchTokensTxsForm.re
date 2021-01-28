open ReactNative;

let style =
  Style.(style(~padding=4.->dp, ~margin=4.->dp, ~borderWidth=1.0, ()));

module TokensAPI = API.Tokens(API.TezosClient);

[@react.component]
let make = () => {
  let settings = ConfigContext.useSettings();
  let (source, setSource) = React.useState(() => "");
  let (token, setToken) = React.useState(() => "");
  let (amount1, setAmount1) = React.useState(() => 0);
  let (destination1, setDestination1) = React.useState(() => "");
  let (amount2, setAmount2) = React.useState(() => 0);
  let (destination2, setDestination2) = React.useState(() => "");

  <View style>
    <TextInput
      onChangeText={text => setSource(_ => text)}
      placeholder="source"
      value=source
    />
    <TextInput
      onChangeText={text => setToken(_ => text)}
      placeholder="token"
      value=token
    />
    <TextInput
      onChangeText={text =>
        text
        ->int_of_string_opt
        ->(x => x->Option.getWithDefault(amount1))
        ->(x => setAmount1(_ => x))
      }
      placeholder="amount1"
      value={amount1 == 0 ? "" : string_of_int(amount1)}
    />
    <TextInput
      onChangeText={text => setDestination1(_ => text)}
      placeholder="destination1"
      value=destination1
    />
    <TextInput
      onChangeText={text =>
        text
        ->int_of_string_opt
        ->(x => x->Option.getWithDefault(amount2))
        ->(x => setAmount2(_ => x))
      }
      placeholder="amount2"
      value={amount2 == 0 ? "" : string_of_int(amount2)}
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
          Token.makeSingleTransferElt(
            ~token,
            ~amount=amount1,
            ~destination=destination1,
            (),
          );
        [@warning "-8"]
        let tx2 =
          Token.makeSingleTransferElt(
            ~token,
            ~amount=amount2,
            ~destination=destination2,
            (),
          );
        Token.(makeTransfers(~source, ~transfers=[tx1, tx2], ())->transfer)
        ->(
            txs => {
              [@warning "-8"]
              let Token.Transfer({transfers}) = txs;
              Js.log(TokensAPI.transfers_to_json(transfers));
              txs;
            }
          )
        ->TokensAPI.simulate(AppSettings.testOnly(settings), _)
        ->Future.get(result =>
            switch (result) {
            | Ok(_) => Dialog.error("ok")
            | Error(err) => Js.log(err)
            }
          );
      }}
      title="Simulate batch token transfer"
    />
  </View>;
};
