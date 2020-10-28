open ChildReprocess.StdStream;

let endpoint = network =>
  switch (network) {
  | Network.Main => "https://mainnet-tezos.giganode.io:443"
  //| Network.Test => "https://testnet-tezos.giganode.io:443"
  | Network.Test => "http://u.lamini.ca/tezos/carthagenet"
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
    ->Future.map(result =>
        result->map(Json.Decode.(array(Operation.decode)))
      );

  let arguments = (network, operation: Injection.operation) =>
    switch (operation) {
    | Transaction(transaction) =>
      let arguments = [|
        "-E",
        network->endpoint,
        "-w",
        "none",
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
      switch (transaction.forceLowFee) {
      | Some(true) => Js.Array2.concat(arguments, [|"--force-low-fee"|])
      | Some(false)
      | None => arguments
      };
    | Delegation(delegation) => [|
        "-E",
        network->endpoint,
        "set",
        "delegate",
        "for",
        delegation.source,
        "to",
        delegation.delegate,
      |]
    };

  type dryRun = {
    fee: float,
    count: int,
    gasLimit: int,
    storageLimit: int,
  };

  let parse = (receipt, pattern) =>
    Js.Re.fromString(pattern)
    ->Js.Re.exec_(receipt)
    ->Belt.Option.map(Js.Re.captures)
    ->Belt.Option.flatMap(captures => captures[1]->Js.Nullable.toOption);

  exception InvalidReceiptFormat;

  let simulate = (network, operation: Injection.operation) =>
    Caller.call(arguments(network, operation)->Js.Array2.concat([|"-D"|]))
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->map(receipt => {
          Js.log("#### #### #### #### #### #### #### ####");
          let fee =
            receipt
            ->parse("[ ]*Fee to the baker: .([0-9]*\.[0-9]+|[0-9]+)")
            ->Belt.Option.flatMap(float_of_string_opt);
          Js.log(fee);
          let count =
            receipt
            ->parse("[ ]*Expected counter: ([0-9]+)")
            ->Belt.Option.flatMap(int_of_string_opt);
          Js.log(count);
          let gasLimit =
            receipt
            ->parse("[ ]*Gas limit: ([0-9]+)")
            ->Belt.Option.flatMap(int_of_string_opt);
          Js.log(gasLimit);
          let storageLimit =
            receipt
            ->parse("[ ]*Storage limit: ([0-9]+)")
            ->Belt.Option.flatMap(int_of_string_opt);
          Js.log(storageLimit);
          switch (fee, count, gasLimit, storageLimit) {
          | (Some(fee), Some(count), Some(gasLimit), Some(storageLimit)) => {
              fee,
              count,
              gasLimit,
              storageLimit,
            }
          | _ => raise(InvalidReceiptFormat)
          };
        })
      )
    ->Future.tapOk(Js.log);

  let create = (network, operation: Injection.operation) =>
    Caller.call(arguments(network, operation))
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->map(receipt => {
          let result = receipt->parse("Operation hash is '([A-Za-z0-9]*)'");
          switch (result) {
          | Some(operationHash) => operationHash
          | None => raise(InvalidReceiptFormat)
          };
        })
      )
    ->Future.tapOk(Js.log);
};

module MapString = Belt.Map.String;

module Accounts = (Caller: CallerAPI) => {
  let parse = content =>
    content
    ->Js.String2.split("\n")
    ->Belt.Array.map(row => row->Js.String2.split(" "))
    ->(pairs => pairs->Belt.Array.keep(data => data->Belt.Array.length > 2))
    ->Belt.Array.map(data =>
        (
          data[0]
          ->Js.String2.substring(
              ~from=0,
              ~to_=data[0]->Js.String2.length - 1,
            ),
          data[1],
        )
      );

  let get = () =>
    Caller.call([|
      "-E",
      Network.Test->endpoint,
      "list",
      "known",
      "addresses",
    |])
    ->Future.mapOk(parse);

  let create = name =>
    Caller.call([|"-E", Network.Test->endpoint, "gen", "keys", name|]);

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
      Caller.call([|
        "-E",
        Network.Test->endpoint,
        "generate",
        "keys",
        "from",
        "mnemonic",
        backupPhrase,
      |])
      ->Future.tapOk(Js.log)
      ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
      ->Future.tapOk(Js.log)
      ->Future.flatMapOk(edsk => import(edsk, name))
    };
  };

  let delete = name =>
    Caller.call([|
      "-E",
      Network.Test->endpoint,
      "forget",
      "address",
      name,
      "-f",
    |]);

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

module Aliases = (Caller: CallerAPI) => {
  let parse = content =>
    content
    |> Js.String.split("\n")
    |> Array.map(row => row |> Js.String.split(": "))
    |> (pairs => pairs->Belt.Array.keep(pair => pair->Array.length == 2))
    |> Array.map(pair => (pair[0], pair[1]));

  let get = () =>
    Caller.call([|
      "-E",
      Network.Test->endpoint,
      "list",
      "known",
      "contracts",
    |])
    ->Future.mapOk(parse);

  let getAliasForAddress = address =>
    get()
    ->Future.mapOk(addresses =>
        addresses->Belt.Array.map(((a, b)) => (b, a))
      )
    ->Future.mapOk(Belt.Map.String.fromArray)
    ->Future.mapOk(aliases => aliases->Belt.Map.String.get(address));

  let getAddressForAlias = alias =>
    get()
    ->Future.mapOk(Belt.Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Belt.Map.String.get(alias));

  let add = (alias, address) =>
    Caller.call([|
      "-E",
      Network.Test->endpoint,
      "add",
      "address",
      alias,
      address,
    |]);

  let delete = name =>
    Caller.call([|
      "-E",
      Network.Test->endpoint,
      "forget",
      "address",
      name,
      "-f",
    |]);
};
