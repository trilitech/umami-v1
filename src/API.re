open ChildReprocess.StdStream;

let endpoint = network =>
  switch (network) {
  | Network.Main => "https://mainnet-tezos.giganode.io:443"
  | Network.Test => "https://testnet-tezos.giganode.io:443"
  };

module URL = {
  let operations =
      (
        network: Network.t,
        account,
        ~types: option(array(string))=?,
        ~limit: option(int)=?,
        (),
      ) =>
    (
      switch (network) {
      | Main => "https://x.lamini.ca/mezos/mainnet/operations?address="
      | Test => "https://u.lamini.ca/mezos/carthagenet/operations?address="
      }
    )
    ++ account
    ++ (
      switch (types) {
      | Some(types) => "&types=" ++ types->Js.Array2.joinWith(",")
      | None => ""
      }
    )
    ++ (
      switch (limit) {
      | Some(limit) => "&limit=" ++ limit->Js.Int.toString
      | None => ""
      }
    );

  let delegates = (network: Network.t) =>
    switch (network) {
    | Main => "https://mainnet-tezos.giganode.io/chains/main/blocks/head/context/delegates\\?active=true"
    | Test => "https://testnet-tezos.giganode.io/chains/main/blocks/head/context/delegates\\?active=true"
    };
};

module type CallerAPI = {
  let call: array(string) => Future.t(Belt.Result.t(string, string));
};

module TezosClient = {
  let call = command =>
    Future.make(resolve => {
      let process = ChildReprocess.spawn("tezos-client", command, ());
      let result: ref(option(Belt.Result.t(string, string))) = ref(None);
      let _ =
        process
        ->child_stderr
        ->Readable.on_data(buffer =>
            result := Some(buffer->Node_buffer.toString->Error)
          );
      let _ =
        process
        ->child_stdout
        ->Readable.on_data(buffer =>
            switch (result^) {
            | Some(Ok(text)) =>
              result :=
                Some(
                  Ok(Js.String2.concat(text, buffer->Node_buffer.toString)),
                )
            | Some(Error(_)) => ()
            | None => result := Some(buffer->Node_buffer.toString->Ok)
            }
          );
      let _ =
        process->ChildReprocess.on_exit((_, _) =>
          resolve(
            switch (result^) {
            | Some(value) => value
            | None => Ok("")
            },
          )
        );
      ();
    });
};

module type GetterAPI = {
  let get: string => Future.t(Belt.Result.t(Js.Json.t, string));
};

module TezosExplorer = {
  let get = url =>
    url
    ->Fetch.fetch
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(response =>
        response->Fetch.Response.json->FutureJs.fromPromise(Js.String.make)
      );
};

module Balance = (Caller: CallerAPI) => {
  let get = (network, account) =>
    Caller.call([|
      "-E",
      network->endpoint,
      "get",
      "balance",
      "for",
      account,
    |]);
};

let map = (result: Belt.Result.t('a, string), transform: 'a => 'b) =>
  try(
    switch (result) {
    | Ok(value) => Ok(transform(value))
    | Error(error) => Error(error)
    }
  ) {
  | Json.ParseError(error) => Error(error)
  | Json.Decode.DecodeError(error) => Error(error)
  | _ => Error("Unknown error")
  };

module Operations = (Caller: CallerAPI, Getter: GetterAPI) => {
  let get =
      (
        network,
        account,
        ~types: option(array(string))=?,
        ~limit: option(int)=?,
        (),
      ) =>
    network
    ->URL.operations(account, ~types?, ~limit?, ())
    ->Getter.get
    ->Future.mapOk(Json.Decode.array(Operation.decode));

  let create = (network, operation: Injection.operation) =>
    (
      switch (operation) {
      | Transaction(transaction) =>
        let arguments = [|
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
        |];
        let arguments =
          switch (transaction.fee) {
          | Some(fee) =>
            Js.Array2.concat(arguments, [|"--fee", fee->Js.Float.toString|])
          | None => arguments
          };
        let arguments =
          switch (transaction.counter) {
          | Some(counter) =>
            Js.Array2.concat(arguments, [|"-C", counter->Js.Int.toString|])
          | None => arguments
          };
        let arguments =
          switch (transaction.gasLimit) {
          | Some(gasLimit) =>
            Js.Array2.concat(arguments, [|"-G", gasLimit->Js.Int.toString|])
          | None => arguments
          };
        let arguments =
          switch (transaction.storageLimit) {
          | Some(storageLimit) =>
            Js.Array2.concat(
              arguments,
              [|"-S", storageLimit->Js.Int.toString|],
            )
          | None => arguments
          };
        let arguments =
          switch (transaction.burnCap) {
          | Some(burnCap) =>
            Js.Array2.concat(
              arguments,
              [|"--burn-cap", burnCap->Js.Float.toString|],
            )
          | None => arguments
          };
        let arguments =
          switch (transaction.forceLowFee) {
          | Some(true) => Js.Array2.concat(arguments, [|"--force-low-fee"|])
          | Some(false)
          | None => arguments
          };
        Caller.call(arguments);
      | Delegation(delegation) =>
        Caller.call([|
          "-E",
          network->endpoint,
          "set",
          "delegate",
          "for",
          delegation.source,
          "to",
          delegation.delegate,
        |])
      }
    )
    ->Future.tapOk(Js.log)
    ->Future.mapOk(receipt => {
        let result =
          Js.Re.fromString("Operation hash is '([A-Za-z0-9]*)'")
          ->Js.Re.exec_(receipt);
        switch (result) {
        | Some(result) =>
          switch (Js.Re.captures(result)[1]->Js.Nullable.toOption) {
          | Some(operationHash) => operationHash
          | None => receipt
          }
        | None => receipt
        };
      })
    ->Future.tapOk(Js.log);
};

module MapString = Belt.Map.String;

module Accounts = (Caller: CallerAPI) => {

  let parseAddresses = content =>
    content
    |> Js.String.split("\n")
    |> Array.map(row => row |> Js.String.split(": "))
    |> (pairs => pairs->Belt.Array.keep(pair => pair->Array.length == 2))
    |> Array.map(pair => (pair[0], pair[1]));

  let get = _ =>
    Caller.call([|"list", "known", "contracts"|])
    ->Future.mapOk(parseAddresses);

  let create = name => Caller.call([|"gen", "keys", name|]);

  let add = (name, address) =>
    Caller.call([|"add", "address", name, address, "-f"|]);

  let import = (key, name) =>
    Caller.call([|"import", "secret", "key", name, "unencrypted:" ++ key|]);

  let restore = (backupPhrase, name, ~derivationPath=?, ()) => {
    switch (derivationPath) {
    | Some(derivationPath) =>
      let seed = HD.BIP39.mnemonicToSeedSync(backupPhrase);
      let edsk2 = HD.seedToPrivateKey(HD.deriveSeed(seed, derivationPath));
      Js.log(edsk2);
      import(edsk2, name);
    | None =>
      Caller.call([|"generate", "keys", "from", "mnemonic", backupPhrase|])
      ->Future.tapOk(Js.log)
      ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
      ->Future.tapOk(Js.log)
      ->Future.flatMapOk(edsk => import(edsk, name))
    };
  };

  let delete = name => Caller.call([|"forget", "address", name, "-f"|]);

  let delegate = (network, account, delegate) =>
    Caller.call([|
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

module Scanner = (Caller: CallerAPI, Getter: GetterAPI) => {
  module AccountsAPI = Accounts(Caller);
  module OperationsAPI = Operations(Caller, Getter);

  let validate = (network, address) => {
    network
    ->OperationsAPI.get(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0});
  };

  let rec scan = (network, backupPhrase, baseName, ~derivationSchema, ~index) => {
    let seed = HD.BIP39.mnemonicToSeedSync(backupPhrase);
    let suffix = index->Js.Int.toString;
    let derivationPath = derivationSchema->Js.String2.replace("?", suffix);
    let edsk2 = HD.seedToPrivateKey(HD.deriveSeed(seed, derivationPath));
    Js.log(baseName ++ index->Js.Int.toString ++ " " ++ edsk2);
    let name = baseName ++ suffix;
    AccountsAPI.import(edsk2, name)
    ->Future.flatMapOk(_ =>
        AccountsAPI.get()
        ->Future.mapOk(MapString.fromArray)
        ->Future.flatMapOk(accounts =>
            switch (accounts->Belt.Map.String.get(name)) {
            | Some(address) =>
              network
              ->validate(address)
              ->Future.flatMapOk(isValidated =>
                  if (isValidated) {
                    scan(
                      network,
                      backupPhrase,
                      baseName,
                      ~derivationSchema,
                      ~index=index + 1,
                    );
                  } else {
                    AccountsAPI.delete(name);
                  }
                )
            | None => Future.make(resolve => resolve(Ok("")))
            }
          )
      );
  };
};

module Delegates = (Getter: GetterAPI) => {
  let get = network =>
    network
    ->URL.delegates
    ->Getter.get
    ->Future.map(result => result->map(Json.Decode.(array(string))));
};
