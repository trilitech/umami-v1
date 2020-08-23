open ChildReprocess.StdStream;

let config = network =>
  switch (network) {
  | Network.Main => "mainnet.json"
  | Network.Test => "testnet.json"
  };

module URL = {
  let transactions = (network: Network.name, account) =>
    (
      switch (network) {
      | Main => "https://mezos.lamini.ca/mezos/mainnet/history?ks="
      | Test => "https://mezos.lamini.ca/mezos/carthagenet/history?ks="
      }
    )
    ++ account;
};

module Balance = {
  let get = (network, account) =>
    Future.make(resolve => {
      let _ =
        ChildReprocess.spawn(
          "tezos-client",
          [|"-c", network->config, "get", "balance", "for", account|],
          (),
        )
        ->child_stdout
        ->Readable.on_data(buffer =>
            buffer->Node_buffer.toString->Belt.Result.Ok->resolve
          );
      ();
    });
};

module Transactions = {
  let get = (network, account) =>
    network
    ->URL.transactions(account)
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      )
    ->Future.mapOk(Json.Decode.(Operation.Decoder.transaction->array->array))
    ->Future.mapOk(value => value[0])
    ->Future.mapOk(transactions => {
        Array.sort(
          (a: Operation.transaction, b) => a.time < b.time ? 1 : (-1),
          transactions,
        );
        transactions;
      })
    ->Future.tapOk(Js.log);

  let post = (network, transaction: Injection.transaction) =>
    Future.make(resolve =>
      ChildReprocess.spawn(
        "tezos-client",
        [|
          "-c",
          network->config,
          "transfer",
          Js.Float.toString(transaction.amount),
          "from",
          transaction.source,
          "to",
          transaction.destination,
          "--burn-cap",
          "0.257",
        |],
        (),
      )
      ->child_stdout
      ->Readable.on_data(buffer => buffer->Node_buffer.toString->Js.log)
      ->Readable.on_close(_ => resolve(Belt.Result.Ok()))
    );
};

module Accounts = {
  let get = () =>
    Future.make(resolve =>
      ChildReprocess.spawn(
        "tezos-client",
        [|"list", "known", "contracts"|],
        (),
      )
      ->child_stdout
      ->Readable.on_data(buffer =>
          buffer->Node_buffer.toString->Belt.Result.Ok->resolve
        )
    );

  let post = name =>
    Future.make(resolve =>
      ChildReprocess.spawn("tezos-client", [|"gen", "keys", name|], ())
      ->child_stdout
      ->Readable.on_close(_ => resolve(Belt.Result.Ok()))
    );
};