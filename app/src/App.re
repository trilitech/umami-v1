open ReactNative;
open ChildReprocess.StdStream;

let styles =
  Style.(
    StyleSheet.create({
      "main": style(
        ~padding=8.->dp,
        ~backgroundColor="#ffffff",
        ()
      ),
      "input": style(
        ~flexDirection=`row,
        ~padding=8.->dp,
        ()
      ),
      "section": style(
        ~padding=8.->dp,
        ()
      ),
      "row": style(
        ~padding=4.->dp,
        ()
      )
    })
  );

type transaction = {
  id: string,
  time: Js.Date.t,
  sender: string,
  receiver: string,
  amount: string,
  fee: string
};

let decodeTransaction = json =>
  Json.Decode.{
    id: json |> field("op_hash", string),
    time: json |> field("timestamp", date),
    sender: json |> field("src", string),
    receiver: json |> field("dst", string),
    amount: json |> field("amount", string),
    fee: json |> field("fee", string)
  };

let fetchTransactions = Js.Promise.(
  Fetch.fetch(
    "https://mezos.lamini.ca/mezos/carthagenet/history?ks=tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
  )
  |> then_(Fetch.Response.json)
  |> then_(json => json |> Json.Decode.array(Json.Decode.array(decodeTransaction)) |> Array.to_list |> Array.concat |> resolve)
);

[@react.component]
let make = () => {
  let (balance, setBalance) = React.useState(() => "");
  let (amount, setAmount) = React.useState(() => 0.0);
  let (destination, setDestination) = React.useState(() => "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3");
  let (transactions, setTransactions) = React.useState(() => [||]);
  
  React.useEffect0(() => {
    let _ = ChildReprocess.spawn("tezos-client", [|"get", "balance", "for", "zebra"|], ())
    ->child_stdout->Readable.on_data(buffer => setBalance(_ => Node_buffer.toString(buffer)));
    None;
  });

  React.useEffect0(() => {
    let _ = Js.Promise.(
      fetchTransactions
      |> then_(transactions =>
        transactions
        |> {transactions => setTransactions(_ => transactions)}
        |> resolve
      )
    );
    None;
  });

  <View style=styles##main>
    <View style=styles##section>
      <Text style=styles##row>{Js.String.concat(balance, "Balance: ") |> React.string}</Text>
    </View>
    <View style=styles##section>
      <TextInput style=styles##row
        onChangeText={text => text |> Js.Float.fromString |> {x => Js.Float.isNaN(x) ? amount : x} |> {x => setAmount(_ => x)}}
        value={Js.Float.toString(amount)}
      />
      <TextInput style=styles##row
        onChangeText={text => setDestination(_ => text)}
        value={destination}
      />
      <Button onPress={_ => {
        let _ = ChildReprocess.spawn("tezos-client", [|"transfer", Js.Float.toString(amount), "from", "zebra", "to", destination|], ())
        ->child_stdout->Readable.on_data(buffer => Js.log(Node_buffer.toString(buffer)));
      }}
      title="Send"
      />
    </View>
    <FlatList style=styles##section
      data=transactions
      keyExtractor={(transaction, _) => transaction.id}
      renderItem={transaction =>
        <View style=styles##row>
          <Text>{Js.String.concat(Js.Date.toLocaleString(transaction.item.time), "Date: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.sender, "Sender: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.receiver, "Receiver: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.amount, "Amount: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.fee, "Fee: ") |> React.string}</Text>
        </View>
      }
    />
  </View>;
};