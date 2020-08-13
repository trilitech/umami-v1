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
      "head": style(
        ~flex=1.,
        ~justifyContent=`spaceBetween,
        ~flexDirection=`row,
        ~padding=8.->dp,
        ()
      ),
      "network": style(
        ~flex=1.,
        ~justifyContent=`flexEnd,
        ~flexDirection=`row,
        ~padding=4.->dp,
        ()
      ),
      "section": style(
        ~padding=8.->dp,
        ()
      ),
      "history": style(
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
  amount: float,
  fee: string
};

let decodeTransaction = json =>
  Json.Decode.{
    id: json |> field("op_hash", string),
    time: json |> field("timestamp", date),
    sender: json |> field("src", string),
    receiver: json |> field("dst", string),
    amount: json |> field("amount", string) |> Js.Float.fromString |> {x => x /. 1000000.},
    fee: json |> field("fee", string)
  };

let fetchTransactions = (main) => Js.Promise.(
  Fetch.fetch(
    main
    ? "https://mezos.lamini.ca/mezos/mainnet/history?ks=tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"
    : "https://mezos.lamini.ca/mezos/carthagenet/history?ks=tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3",
  )
  |> then_(Fetch.Response.json)
  |> then_(json => json |> Json.Decode.array(Json.Decode.array(decodeTransaction))
                        |> Array.to_list
                        |> Array.concat
                        |> {x => {Array.sort((a, b) => a.time > b.time ? -1 : 1, x); x}}
                        |> resolve)
);

[@react.component]
let make = () => {
  let (balance, setBalance) = React.useState(() => "");
  let (amount, setAmount) = React.useState(() => 1.0);
  let (destination, setDestination) = React.useState(() => "bob");
  let (transactions, setTransactions) = React.useState(() => [||]);
  let (name, setName) = React.useState(() => "bob");
  let (contracts, setContracts) = React.useState(() => "");
  let (main, setMain) = React.useState(() => false)
  
  let network = main ? "mainnet.json" : "testnet.json";

  React.useEffect1(() => {
    let _ = ChildReprocess.spawn("tezos-client", [|"-c", network, "get", "balance", "for", "zebra"|], ())
    ->child_stdout->Readable.on_data(buffer => setBalance(_ => Node_buffer.toString(buffer)));
    None;
  }, [|main|]);

  React.useEffect1(() => {
    let _ = ChildReprocess.spawn("tezos-client", [|"list", "known", "contracts"|], ())
    ->child_stdout->Readable.on_data(buffer => setContracts(_ => buffer->Node_buffer.toString));
    None;
  }, [|main|]);

  React.useEffect1(() => {
    let _ = Js.Promise.(
      fetchTransactions(main)
      |> then_(transactions =>
        transactions
        |> {transactions => setTransactions(_ => transactions)}
        |> resolve
      )
    );
    None;
  }, [|main|]);

  <SafeAreaView style=styles##main>
    <View style=styles##head>
      <Text style=styles##row>{("Balance: " ++ balance)->React.string}</Text>
      <View style=styles##network>
        <Text>"testnet  "->React.string</Text>
        <Switch onValueChange={value => setMain(_ => value)} value={main}/>
        <Text>"  mainnet"->React.string</Text>
      </View>
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
        let command = [|"-c", network, "transfer", Js.Float.toString(amount), "from", "zebra", "to", destination, "--burn-cap", "0.257"|];
        let stream = ChildReprocess.spawn("tezos-client", command, ())->child_stdout;
        let _ = stream->Readable.on_data(buffer => Js.log(buffer->Node_buffer.toString));
        let _ = stream->Readable.on_close(_ => {
          let _ = ChildReprocess.spawn("tezos-client", [|"-c", network, "get", "balance", "for", "zebra"|], ())
          ->child_stdout
          ->Readable.on_data(buffer => setBalance(_ => Node_buffer.toString(buffer)));
          let _ = fetchTransactions(main)
          |> Js.Promise.then_(transactions => transactions |> {transactions => setTransactions(_ => transactions)} |> Js.Promise.resolve)
          |> Js.Promise.resolve;
        });
      }}
      title="Send"
      />
    </View>
    <View style=styles##section>
      <TextInput style=styles##row
        onChangeText={text => setName(_ => text)}
        value={name}
      />
      <Button onPress={_ => {
        let stream = ChildReprocess.spawn("tezos-client", [|"gen", "keys", name|], ())->child_stdout;
        let _ = stream->Readable.on_data(buffer => Js.log(Node_buffer.toString(buffer)));
        let _ = stream->Readable.on_close(_ => {
          let _ = ChildReprocess.spawn("tezos-client", [|"list", "known", "contracts"|], ())
          ->child_stdout->Readable.on_data(buffer => setContracts(_ => buffer->Node_buffer.toString));
        });
      }}
      title="Create"
      />
      <Text style=styles##row>{contracts->React.string}</Text>
    </View>
    <View style=styles##history>
      <Button onPress={_ => {
          let _ = fetchTransactions(main)
          |> Js.Promise.then_(transactions => transactions |> {transactions => setTransactions(_ => transactions)} |> Js.Promise.resolve)
          |> Js.Promise.resolve;
        }}
        title="Refresh"
      />
      <FlatList
        data=transactions
        keyExtractor={(transaction, _) => transaction.id}
        renderItem={transaction =>
          <View style=styles##row>
            <Text>{("Date: " ++ Js.Date.toLocaleString(transaction.item.time))->React.string}</Text>
            <Text>{("Sender: " ++ transaction.item.sender)->React.string}</Text>
            <Text>{("Receiver: " ++ transaction.item.receiver)->React.string}</Text>
            <Text>{("Amount: " ++ Js.Float.toString(transaction.item.amount) ++ " " ++ {js|ꜩ|js})->React.string}</Text>
            <Text>{("Fee: " ++ transaction.item.fee ++ " " ++ {js|μꜩ|js})->React.string}</Text>
          </View>
        }
      />
    </View>
  </SafeAreaView>;
};