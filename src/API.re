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

  let delegates = (network: Network.name) =>
    switch (network) {
    | Main => "https://mainnet-tezos.giganode.io/chains/main/blocks/head/context/delegates\\?active=true"
    | Test => "https://testnet-tezos.giganode.io/chains/main/blocks/head/context/delegates\\?active=true"
    };
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
        )
      ->Readable.on_close(_ => resolve(Belt.Result.Ok("")));
    ();
  })
  ->Future.tapOk(Js.log)
  ->Future.tapError(Js.log);

module type ClientAPI = {
  let call: array(string) => Future.t(Belt.Result.t(string, string));
};

module TezosClient = {
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
          )
        ->Readable.on_close(_ => resolve(Belt.Result.Ok("")));
      ();
    })
    ->Future.tapOk(Js.log)
    ->Future.tapError(Js.log);
};

module type FetchAPI = {
  let fetch: string => Future.t(Belt.Result.t(Js.Json.t, string));
};

module TezosExplorer = {
  let fetch = url =>
    url
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      );
};

module Balance = (API: ClientAPI) => {
  let get = (network, account) =>
    API.call([|"-E", network->endpoint, "get", "balance", "for", account|]);
};

let map = (result: Belt.Result.t('a, string), transform: ('a) => 'b) =>  {
  try (switch (result) {
    | Ok(value) => Ok(transform(value))
    | Error(error) => Error(error) 
  }) {
    | Json.ParseError(error) => Error(error)
    | Json.Decode.DecodeError(error) => Error(error)
    | _ => Error("Unknown error")
  }
};

module Operations = (API: FetchAPI) => {
  let get = (network, account) =>
    network
    ->URL.operations(account)
    ->API.fetch
    ->Future.map(result => result->map(Json.Decode.array(Operation.decode)))
    ->Future.tapOk(Js.log);

  let create = (network, operation: Injection.operation) =>
    switch (operation) {
    | Transaction(transaction) =>
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
      |])
    | Delegation(delegation) =>
      call([|
        "-E",
        network->endpoint,
        "set",
        "delegate",
        "for",
        delegation.source,
        "to",
        delegation.delegate,
      |])
    };
};

module MapString = Belt.Map.String;

let parseAddresses = content =>
  content
  |> Js.String.split("\n")
  |> Array.map(row => row |> Js.String.split(": "))
  |> (pairs => pairs->Belt.Array.keep(pair => pair->Array.length == 2))
  |> Array.map(pair => (pair[0], pair[1]))
  |> MapString.fromArray;

module Accounts = {
  let get = _ =>
    call([|"list", "known", "contracts"|])->Future.mapOk(parseAddresses);

  let create = name => call([|"gen", "keys", name|]);

  let add = (name, address) =>
    call([|"add", "address", name, address, "-f"|]);

  let restore = (backupPhrase, name) =>
    call([|"generate", "keys", "from", "mnemonic", backupPhrase|])
    ->Future.tapOk(Js.log)
    ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
    ->Future.tapOk(Js.log)
    ->Future.flatMapOk(edsk =>
        call([|"import", "secret", "key", name, "unencrypted:" ++ edsk|])
      );

  let delete = name => call([|"forget", "address", name, "-f"|]);

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

  module Delegates = (API: FetchAPI) => {
    let get = network =>
      network
      ->URL.delegates
      ->API.fetch
      ->Future.map(result => result->map(Json.Decode.(array(string))))
      ->Future.tapOk(Js.log);
  };
};