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

  let create = (network, transaction: Injection.transaction) =>
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

module MapString = Belt.Map.String;

let parse = output =>
  output
  |> Js.String.split("\n")
  |> Array.map(row => row |> Js.String.split(": "))
  |> (pairs => pairs->Belt.Array.keep(pair => pair->Array.length == 2))
  |> Array.map(pair => (pair[0], pair[1]))
  |> MapString.fromArray;

module Accounts = {
  let get = () =>
    Future.make(resolve => {
      let process =
        ChildReprocess.spawn(
          "tezos-client",
          [|"list", "known", "contracts"|],
          (),
        );
      let _ =
        process
        ->child_stdout
        ->Readable.on_data(buffer =>
            buffer->Node_buffer.toString->Belt.Result.Ok->resolve
          );
      ();
    })
    ->Future.mapOk(parse);

  let create = name =>
    Future.make(resolve => {
      let process =
        ChildReprocess.spawn("tezos-client", [|"gen", "keys", name|], ());
      let _ =
        process
        ->child_stderr
        ->Readable.on_data(buffer =>
            buffer->Node_buffer.toString->Belt.Result.Error->resolve
          );
      let _ =
        process
        ->child_stdout
        ->Readable.on_close(_ => resolve(Belt.Result.Ok()));
      ();
    });

  let restore = (backupPhrase, name) =>
    backupPhrase
    ->BIP39.mnemonicToSeedSync
    ->Crypto.edsk
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.tapOk(Js.log)
    ->Future.flatMapOk(edsk =>
        Future.make(resolve => {
          let process =
            ChildReprocess.spawn(
              "tezos-client",
              [|"import", "secret", "key", name, "unencrypted:" ++ edsk|],
              (),
            );
          let _ =
            process
            ->child_stderr
            ->Readable.on_data(buffer =>
                buffer->Node_buffer.toString->Belt.Result.Error->resolve
              );
          let _ =
            process
            ->child_stdout
            ->Readable.on_close(_ => resolve(Belt.Result.Ok()));
          ();
        })
      );

  let delete = name =>
    Future.make(resolve => {
      let process =
        ChildReprocess.spawn(
          "tezos-client",
          [|"forget", "address", name, "--force"|],
          (),
        );
      let _ =
        process
        ->child_stderr
        ->Readable.on_data(buffer =>
            buffer->Node_buffer.toString->Belt.Result.Error->resolve
          );
      let _ =
        process
        ->child_stdout
        ->Readable.on_close(_ => resolve(Belt.Result.Ok()));
      ();
    });
};