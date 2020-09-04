open ChildReprocess.StdStream;

let endpoint = network =>
  switch (network) {
  | Network.Main => "https://mainnet-tezos.giganode.io:443"
  | Network.Test => "https://testnet-tezos.giganode.io:443"
  };

module URL = {
  let operations = (network: Network.name, account) =>
    (
      switch (network) {
      | Main => "https://x.lamini.ca/mezos/mainnet/operations?address="
      | Test => "https://u.lamini.ca/mezos/carthagenet/operations?address="
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
          [|"-c", network->endpoint, "get", "balance", "for", account|],
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
    ->URL.operations(account)
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      )
    ->Future.mapOk(Json.Decode.array(Operation.decode))
    ->Future.tapOk(Js.log);

  let create = (network, transaction: Injection.transaction) =>
    Future.make(resolve => {
      let process =
        ChildReprocess.spawn(
          "tezos-client",
          [|
            "-E",
            network->endpoint,
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
        ->Readable.on_data(buffer => buffer->Node_buffer.toString->Js.log)
        ->Readable.on_close(_ => resolve(Belt.Result.Ok()));
      ();
    })
    ->Future.tapOk(Js.log);
};

module MapString = Belt.Map.String;

let parseAddresses = output =>
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
    ->Future.mapOk(parseAddresses);

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
    Future.make(resolve => {
      let process =
        ChildReprocess.spawn(
          "tezos-client",
          [|"generate", "keys", "from", "mnemonic", backupPhrase|],
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
        ->Readable.on_data(buffer =>
            buffer->Node_buffer.toString->Belt.Result.Ok->resolve
          );
      ();
    })
    ->Future.tapOk(Js.log)
    ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
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
