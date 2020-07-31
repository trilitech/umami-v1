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
  id: int,
  time: Js.Date.t,
  sender: string,
  receiver: string,
  amount: float,
  fee: float
};

let decodeTransaction = json =>
  Json.Decode.{
    id: json |> field("row_id", int),
    time: json |> field("time", date),
    sender: json |> field("sender", string),
    receiver: json |> field("receiver", string),
    amount: json |> field("volume", Json.Decode.float),
    fee: json |> field("fee", Json.Decode.float)
  };

let fetchTransactions = Js.Promise.(
  Fetch.fetch("https://api.carthagenet.tzstats.com/explorer/account/tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3/operations?type=transaction&order=desc&limit=8")
  |> then_(Fetch.Response.json)
  |> then_(json => json |> Json.Decode.array(decodeTransaction) |> resolve)
);

[@react.component]
let make = () => {
  let (balance, setBalance) = React.useState(() => "");
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
    <FlatList style=styles##section
      data=transactions
      keyExtractor={(transaction, _) => string_of_int(transaction.id)}
      renderItem={transaction =>
        <View style=styles##row>
          <Text>{Js.String.concat(Js.Date.toLocaleString(transaction.item.time), "Date: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.sender, "Sender: ") |> React.string}</Text>
          <Text>{Js.String.concat(transaction.item.receiver, "Receiver: ") |> React.string}</Text>
          <Text>{Js.String.concat(Js.Float.toString(transaction.item.amount), "Amount: ") |> React.string}</Text>
          <Text>{Js.String.concat(Js.Float.toString(transaction.item.fee), "Fee: ") |> React.string}</Text>
        </View>
      }
    />
  </View>;
};