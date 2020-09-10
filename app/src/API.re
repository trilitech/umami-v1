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

let call = command =>
  Future.make(resolve => {
    let process = ChildReprocess.spawn("tezos-client", command, ());
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
  ->Future.tap(Js.log);

module Balance = {
  let get = (network, account) =>
    call([|"-E", network->endpoint, "get", "balance", "for", account|]);
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
    call([|
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
    |]);
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
    call([|"list", "known", "contracts"|])->Future.mapOk(parseAddresses);

  let create = name => call([|"gen", "keys", name|]);

  let restore = (backupPhrase, name) =>
    call([|"generate", "keys", "from", "mnemonic", backupPhrase|])
    ->Future.tapOk(Js.log)
    ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
    ->Future.tapOk(Js.log)
    ->Future.flatMapOk(edsk =>
        call([|"import", "secret", "key", name, "unencrypted:" ++ edsk|])
      );

  let delete = name => call([|"forget", "address", name, "--force"|]);

  let delegate = (network, account, delegate) =>
    call([|
      "-E",
      network->endpoint,
      "set",
      "delegate",
      "for",
      account,
      "to",
      delegate,
    |]);
};
