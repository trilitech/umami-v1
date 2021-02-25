open ChildReprocess.StdStream;
open UmamiCommon;
open Delegate;

module Path = {
  let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
  let operations = "operations";
  let mempool_operations = "mempool/accounts";
  let tokenViewer = "tokens/viewer";
};

module URL = {
  let arg_opt = (v, n, f) => v->Option.map(a => (n, f(a)));

  let build_args = l =>
    l->List.map(((a, v)) => a ++ "=" ++ v)->List.toArray
    |> Js.Array.joinWith("&");

  let build_url = (network, path, args) => {
    AppSettings.explorer(network)
    ++ "/"
    ++ path
    ++ (args == [] ? "" : "?" ++ args->build_args);
  };

  let operations =
      (
        settings: AppSettings.t,
        account,
        ~types: option(array(string))=?,
        ~destination: option(string)=?,
        ~limit: option(int)=?,
        (),
      ) => {
    let operationsPath = "accounts/" ++ account ++ "/operations";
    let args =
      Lib.List.(
        []
        ->addOpt(destination->arg_opt("destination", dst => dst))
        ->addOpt(limit->arg_opt("limit", lim => lim->Js.Int.toString))
        ->addOpt(types->arg_opt("types", t => t->Js.Array2.joinWith(",")))
      );
    let url = build_url(settings, operationsPath, args);
    url;
  };

  let mempool = (network, account) => {
    let path = Path.mempool_operations ++ "/" ++ account ++ "/operations";
    build_url(network, path, []);
  };

  let tokenViewer = network => build_url(network, Path.tokenViewer, []);

  let delegates = settings =>
    AppSettings.endpoint(settings) ++ Path.delegates;
};

module type CallerAPI = {
  let call:
    (array(string), ~inputs: array(string)=?, unit) =>
    Future.t(Result.t(string, string));
};

module TezosClient = {
  [@bs.send] external end_: Writeable.t => unit = "end";

  let removeTestnetWarning = output => {
    let warning =
      Js.Re.fromString(
        "[ ]*Warning:[ \n]*This is NOT the Tezos Mainnet.[ \n]*\
       Do NOT use your fundraiser keys on this network.[ \n]*",
      );
    Js.String.replaceByRe(warning, "", output);
  };

  let call = (command, ~inputs=?, ()) =>
    Future.make(resolve => {
      let process = ChildReprocess.spawn("tezos-client", command, ());
      let result: ref(option(Result.t(string, string))) = ref(None);
      let _ =
        process
        ->child_stderr
        ->Readable.on_data(buffer => {
            let err = Node_buffer.toString(buffer);
            result :=
              removeTestnetWarning(err) == "" ? None : Some(Error(err));
          });
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
        switch (inputs) {
        | Some(inputs) =>
          process
          ->child_stdin
          ->Writeable.write(inputs->Js.Array2.joinWith("\n") ++ "\n\000");
          process->child_stdin->end_;
        | None => ()
        };
      let _ =
        process->ChildReprocess.on_close((_, _) =>
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
  let get: string => Future.t(Result.t(Js.Json.t, string));
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
  let get = (settings, account, ~block=?, ()) => {
    let arguments = [|
      "-E",
      settings->AppSettings.endpoint,
      "get",
      "balance",
      "for",
      account,
    |];
    let arguments =
      switch (block) {
      | Some(block) => Js.Array2.concat([|"-b", block|], arguments)
      | None => arguments
      };

    Caller.call(arguments, ())
    ->Future.flatMapOk(r =>
        r
        ->Float.fromString
        ->Option.map(Float.toString)
        ->Option.flatMap(ProtocolXTZ.fromString)
        ->(
            ropt =>
              switch (ropt) {
              | None when r == "" => Ok(ProtocolXTZ.zero)
              | None => Error("Cannot parse balance")
              | Some(r) => Ok(r)
              }
          )
        ->Future.value
      );
  };
};

let tryMap = (result: Result.t('a, string), transform: 'a => 'b) =>
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

module InjectorRaw = (Caller: CallerAPI) => {
  type parserInterpret('a) =
    | AllReduce(('a, option(string)) => 'a, 'a)
    | First(string => option('a))
    | Last(string => option('a))
    | Nth(string => option('a), int)
    | Exists(string => option('a));

  let parse = (receipt, pattern, interp) => {
    let rec parseAll = (acc, regexp) =>
      switch (
        Js.Re.exec_(regexp, receipt)->Option.map(Js.Re.captures),
        interp,
      ) {
      | (Some(res), Exists(_)) => [|
          res[0]->Option.flatMap(Js.Nullable.toOption),
        |]
      | (Some(res), _) =>
        acc
        ->Js.Array2.concat([|res[1]->Option.flatMap(Js.Nullable.toOption)|])
        ->parseAll(regexp)
      | (None, _) => acc
      };
    let interpretResults = results =>
      if (Js.Array.length(results) == 0) {
        None;
      } else {
        switch (interp) {
        | Exists(f) => results[0]->Option.flatMap(x => x)->Option.flatMap(f)
        | First(f) => results[0]->Option.flatMap(x => x)->Option.flatMap(f)
        | Last(f) =>
          results[Js.Array.length(results) - 1]
          ->Option.flatMap(x => x)
          ->Option.flatMap(f)
        | Nth(f, i) =>
          i < Js.Array.length(results)
            ? results[i]->Option.flatMap(x => x)->Option.flatMap(f) : None
        | AllReduce(f, init) => Some(Js.Array2.reduce(results, f, init))
        };
      };
    parseAll([||], Js.Re.fromStringWithFlags(pattern, ~flags="g"))
    ->interpretResults;
  };

  let parseAndReduceXTZ = (f, s) => {
    Option.mapWithDefault(s, Some(ProtocolXTZ.zero), ProtocolXTZ.fromString)
    ->Option.mapWithDefault(f, ProtocolXTZ.Infix.(+)(f));
  };

  let parseAndReduceInt = (f, s) => {
    Option.mapWithDefault(s, Some(0), int_of_string_opt)
    ->Option.mapWithDefault(f, Int.(+)(f));
  };

  type parserOptions = {
    fees: parserInterpret(ProtocolXTZ.t),
    counter: parserInterpret(int),
    gasLimit: parserInterpret(int),
    storageLimit: parserInterpret(int),
  };

  let singleOperationParser = {
    fees: AllReduce(parseAndReduceXTZ, ProtocolXTZ.zero),
    counter: First(int_of_string_opt),
    gasLimit: AllReduce(parseAndReduceInt, 0),
    storageLimit: AllReduce(parseAndReduceInt, 0),
  };

  let singleBatchTransferParser = index => {
    Js.log(
      "FEE "
      ++ Js.String.make(index)
      ++ " FROM "
      ++ Js.String.make(Nth(float_of_string_opt, index)),
    );

    {
      fees: Nth(ProtocolXTZ.fromString, index),
      counter: Nth(int_of_string_opt, index),
      gasLimit: Nth(int_of_string_opt, index),
      storageLimit: Nth(int_of_string_opt, index),
    };
  };

  let patchNthForRevelation = (options, revelation) => {
    let incrNth = nth =>
      switch (nth) {
      | Nth(f, i) => Nth(f, i + 1)
      | _ => nth
      };
    switch (revelation) {
    | None => options
    | Some () => {
        fees: incrNth(options.fees),
        counter: incrNth(options.counter),
        gasLimit: incrNth(options.gasLimit),
        storageLimit: incrNth(options.storageLimit),
      }
    };
  };

  let transaction_options_arguments =
      (
        arguments,
        tx_options: Protocol.transfer_options,
        common_options: Protocol.common_options,
      ) => {
    let arguments =
      switch (tx_options.fee) {
      | Some(fee) =>
        Js.Array2.concat(arguments, [|"--fee", fee->ProtocolXTZ.toString|])
      | None => arguments
      };
    let arguments =
      switch (tx_options.gasLimit) {
      | Some(gasLimit) =>
        Js.Array2.concat(arguments, [|"-G", gasLimit->Js.Int.toString|])
      | None => arguments
      };
    let arguments =
      switch (tx_options.storageLimit) {
      | Some(storageLimit) =>
        Js.Array2.concat(arguments, [|"-S", storageLimit->Js.Int.toString|])
      | None => arguments
      };
    let arguments =
      switch (common_options.burnCap) {
      | Some(burnCap) =>
        Js.Array2.concat(
          arguments,
          [|"--burn-cap", burnCap->ProtocolXTZ.toString|],
        )
      | None => arguments
      };
    switch (common_options.forceLowFee) {
    | Some(true) => Js.Array2.concat(arguments, [|"--force-low-fee"|])
    | Some(false)
    | None => arguments
    };
  };

  exception InvalidReceiptFormat;

  let simulate = (network, parser_options, make_arguments) =>
    Caller.call(
      make_arguments(network)->Js.Array2.concat([|"--simulation"|]),
      (),
    )
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->tryMap(receipt => {
          let revelation =
            receipt->parse(
              "[ ]*Revelation of manager public key:",
              Exists(_ => Some()),
            );
          Js.log(revelation);
          let parser_options =
            patchNthForRevelation(parser_options, revelation);
          let fee =
            receipt->parse(
              "[ ]*Fee to the baker: .([0-9]*\\.[0-9]+|[0-9]+)",
              parser_options.fees,
            );
          Js.log(fee);
          let count =
            receipt->parse(
              "[ ]*Expected counter: ([0-9]+)",
              parser_options.counter,
            );
          Js.log(count);
          let gasLimit =
            receipt->parse(
              "[ ]*Gas limit: ([0-9]+)",
              parser_options.gasLimit,
            );
          Js.log(gasLimit);
          let storageLimit =
            receipt->parse(
              "[ ]*Storage limit: ([0-9]+)",
              parser_options.storageLimit,
            );
          Js.log(storageLimit);
          switch (fee, count, gasLimit, storageLimit) {
          | (Some(fee), Some(count), Some(gasLimit), Some(storageLimit)) =>
            Protocol.{fee, count, gasLimit, storageLimit}
          | _ => raise(InvalidReceiptFormat)
          };
        })
      )
    ->Future.tapOk(Js.log);

  let create = (network, make_arguments) =>
    Caller.call(make_arguments(network), ())
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->tryMap(receipt => {
          let result =
            receipt->parse(
              "Operation hash is '([A-Za-z0-9]*)'",
              First(s => Some(s)),
            );
          switch (result) {
          | Some(operationHash) => operationHash
          | None => raise(InvalidReceiptFormat)
          };
        })
      )
    ->Future.tapOk(Js.log);

  let inject = (network, make_arguments, ~password) =>
    Caller.call(make_arguments(network), ~inputs=[|password|], ())
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->tryMap(receipt => {
          let operationHash =
            receipt->parse(
              "Operation hash is '([A-Za-z0-9]+)'",
              First(s => Some(s)),
            );
          let branch =
            receipt->parse("--branch ([A-Za-z0-9]+)", First(s => Some(s)));
          switch (operationHash, branch) {
          | (Some(operationHash), Some(branch)) => (operationHash, branch)
          | (_, _) => raise(InvalidReceiptFormat)
          };
        })
      );
};

module Operations = (Caller: CallerAPI, Getter: GetterAPI) => {
  module Injector = InjectorRaw(Caller);

  let getFromMempool = (account, network, operations) =>
    network
    ->URL.mempool(account)
    ->Getter.get
    ->Future.map(result =>
        result->tryMap(x =>
          (
            operations,
            x |> Json.Decode.(array(Operation.Read.decodeFromMempool)),
          )
        )
      )
    >>= (
      ((operations, mempool)) => {
        module Comparator = Operation.Read.Comparator;
        let operations =
          Set.fromArray(operations, ~id=(module Operation.Read.Comparator));

        let operations =
          mempool->Array.reduce(operations, Set.add)->Set.toArray;

        Future.value(Ok(operations));
      }
    );

  let get =
      (
        network,
        account,
        ~types: option(array(string))=?,
        ~destination: option(string)=?,
        ~limit: option(int)=?,
        ~mempool: bool=false,
        (),
      ) =>
    network
    ->URL.operations(account, ~types?, ~destination?, ~limit?, ())
    ->Getter.get
    ->Future.map(result =>
        result->tryMap(Json.Decode.(array(Operation.Read.decode)))
      )
    >>= (
      operations =>
        mempool
          ? getFromMempool(account, network, operations)
          : Future.value(Ok(operations))
    );

  let transfer = (tx: Protocol.transfer) => {
    let obj = Js.Dict.empty();
    Js.Dict.set(obj, "destination", Js.Json.string(tx.destination));
    Js.Dict.set(
      obj,
      "amount",
      tx.amount->ProtocolXTZ.toString->Js.Json.string,
    );
    tx.tx_options.fee
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "fee", v->ProtocolXTZ.toString->Js.Json.string)
      );
    tx.tx_options.gasLimit
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "gas-limit", Js.Int.toString(v)->Js.Json.string)
      );
    tx.tx_options.storageLimit
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "storage-limit", Js.Int.toString(v)->Js.Json.string)
      );
    Js.Json.object_(obj);
  };

  let transfers_to_json = (btxs: Protocol.transaction) =>
    btxs.transfers
    ->List.map(transfer)
    ->List.toArray
    ->Js.Json.array
    ->Js.Json.stringify /* ->(json => "\'" ++ json ++ "\'") */;

  let arguments = (settings, operation: Protocol.t) =>
    switch (operation) {
    | Transaction({transfers: [transfer]} as transaction) =>
      let arguments = [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "transfer",
        transfer.amount->ProtocolXTZ.toString,
        "from",
        transaction.source,
        "to",
        transfer.destination,
        "--burn-cap",
        "0.257",
      |];
      Injector.transaction_options_arguments(
        arguments,
        transfer.tx_options,
        transaction.options,
      );
    | Transaction(transaction) => [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "multiple",
        "transfers",
        "from",
        transaction.source,
        "using",
        transfers_to_json(transaction),
        "--burn-cap",
        Js.Float.toString(
          0.06425 *. transaction.transfers->List.length->float_of_int,
        ),
      |]
    | Delegation({delegate: None, source}) => [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "withdraw",
        "delegate",
        "from",
        source,
      |]
    | Delegation({delegate: Some(delegate), source}) => [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "set",
        "delegate",
        "for",
        source,
        "to",
        delegate,
      |]
    };

  exception InvalidReceiptFormat;

  let simulateSingleBatchTransfer = (settings, index, operation: Protocol.t) =>
    Injector.simulate(
      settings,
      Injector.singleBatchTransferParser(index),
      arguments(_, operation),
    );

  let simpleSimulation = (settings, operation: Protocol.t) =>
    Injector.simulate(
      settings,
      Injector.singleOperationParser,
      arguments(_, operation),
    );

  let simulate = (settings, ~index=?, operation: Protocol.t) => {
    switch (operation, index) {
    | (Delegation(_), _)
    | (Transaction(_), None)
    | (Transaction({transfers: [_]}), _) =>
      simpleSimulation(settings, operation)
    | (Transaction(_), Some(index)) =>
      simulateSingleBatchTransfer(settings, index, operation)
    };
  };

  let create = (network, operation: Protocol.t) =>
    Injector.create(network, arguments(_, operation));

  let inject = (network, operation: Protocol.t, ~password) =>
    Injector.inject(network, arguments(_, operation), ~password);

  let waitForOperationConfirmations =
      (settings, hash, ~confirmations, ~branch) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "wait",
        "for",
        hash,
        "to",
        "be",
        "included",
        "--confirmations",
        confirmations->string_of_int,
        "--branch",
        branch,
      |],
      (),
    );
};

module MapString = Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module Secret = {
  type t = {
    name: string,
    derivationScheme: string,
    addresses: Js.Array.t(string),
    legacyAddress: option(string),
  };

  let decoder = json =>
    Json.Decode.{
      name: json |> field("name", string),
      derivationScheme: json |> field("derivationScheme", string),
      addresses: json |> field("addresses", array(string)),
      legacyAddress: json |> optional(field("legacyAddress", string)),
    };

  let encoder = secret =>
    Json.Encode.(
      switch (secret.legacyAddress) {
      | Some(legacyAddress) =>
        object_([
          ("name", string(secret.name)),
          ("derivationScheme", string(secret.derivationScheme)),
          ("addresses", stringArray(secret.addresses)),
          ("legacyAddress", string(legacyAddress)),
        ])
      | None =>
        object_([
          ("name", string(secret.name)),
          ("derivationScheme", string(secret.derivationScheme)),
          ("addresses", stringArray(secret.addresses)),
        ])
      }
    );
};

module Accounts = (Caller: CallerAPI, Getter: GetterAPI) => {
  let secrets = (~settings: AppSettings.t) => {
    LocalStorage.getItem("secrets")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(Secret.decoder)));
  };

  let recoveryPhrases = (~settings: AppSettings.t, password) =>
    SecureStorage.fetch("recoveryPhrases", ~password)
    ->Future.mapOk(Json.parse)
    ->Future.mapOk(result =>
        result->Option.map(Json.Decode.(array(string)))
      );

  let parse = content =>
    content
    ->Js.String2.split("\n")
    ->Array.map(row => row->Js.String2.split(":"))
    ->(rows => rows->Array.keep(data => data->Array.length >= 2))
    ->Array.map(pair => {
        [|pair->Array.getUnsafe(0)|]
        ->Array.concat(
            pair
            ->Array.getUnsafe(1)
            ->Js.String2.trim
            ->Js.String2.split(" ("),
          )
      })
    ->(rows => rows->Array.keep(data => data->Array.length > 2))
    ->Array.map(data => (data[0], data[1]));
  /*
     let get = (~settings: AppSettings.t) =>
       settings
       ->AppSettings.sdk
       ->TezosSDK.listKnownAddresses
       ->Future.mapOk(r =>
           r->Array.keepMap((TezosSDK.OutputAddress.{alias, pkh, sk_known}) =>
             sk_known ? Some((alias, pkh)) : None
           )
         );
   */
  let get = (~settings: AppSettings.t) =>
    Result.Ok([|
      ("account 0", "tz1dyX3B1CFYa2DfdFLyPtiJCfQRUgPVME6E"),
      ("account 1", "tz1VTfGqp34NypRQJmjNiPrCTG5TRonevsmf"),
      ("zebra", "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"),
    |])
    ->Future.value;

  let create = (~settings, name) =>
    Caller.call(
      [|"-E", settings->AppSettings.endpoint, "gen", "keys", name|],
      (),
    );

  let secretAt = (~settings, index) =>
    secrets(~settings)
    ->FutureEx.fromOption(~error="No secrets found!")
    ->Future.flatMapOk(secrets =>
        secrets[index]
        ->FutureEx.fromOption(
            ~error="Secret at index " ++ index->Int.toString ++ " not found!",
          )
      );

  let updateSecretAt = (secret, ~settings, index) =>
    secrets(~settings)
    ->FutureEx.fromOption(~error="No secrets found!")
    ->Future.flatMapOk(secrets => {
        (secrets[index] = secret)
          ? Future.value(
              Ok(
                LocalStorage.setItem(
                  "secrets",
                  Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
                ),
              ),
            )
          : Future.value(
              Error(
                "Can't update secret at index " ++ index->Int.toString ++ "!",
              ),
            )
      });

  let recoveryPhraseAt = (~settings, index, ~password) =>
    recoveryPhrases(~settings, password)
    ->Future.flatMapOk(recoveryPhrases =>
        recoveryPhrases->FutureEx.fromOption(
          ~error="No recovery phrases found!",
        )
      )
    ->Future.flatMapOk(recoveryPhrases =>
        recoveryPhrases[index]
        ->FutureEx.fromOption(
            ~error=
              "Recovery phrase at index "
              ++ index->Int.toString
              ++ " not found!",
          )
      );

  let derive = (~settings, name, ~index, ~password) =>
    Js.Promise.all2((
      secretAt(~settings, index)->FutureJs.toPromise,
      recoveryPhraseAt(~settings, index, ~password)->FutureJs.toPromise,
    ))
    ->FutureJs.fromPromise(Js.String.make)
    ->Future.flatMapOk(((secret, recoveryPhrase)) => {
        switch (secret, recoveryPhrase) {
        | (Ok(secret), Ok(recoveryPhrase)) =>
          HD.edesk(
            secret.derivationScheme,
            recoveryPhrase->HD.seed,
            ~password,
          )
        | (Error(error), _)
        | (_, Error(error)) => Future.value(Error(error))
        }
      });

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let import = (key, name) =>
    Caller.call(
      [|"import", "secret", "key", name, "encrypted:" ++ key|],
      (),
    );

  let delete = (~settings, name) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "forget",
        "address",
        name,
        "-f",
      |],
      (),
    );

  let validate = (network, address) => {
    module OperationsAPI = Operations(Caller, Getter);
    network
    ->OperationsAPI.get(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0});
  };

  let rec scan =
          (
            ~settings: AppSettings.t,
            seed,
            baseName,
            ~derivationScheme="m/44'/1729'/?'/0'",
            ~password,
            ~index=0,
            (),
          ) => {
    let suffix = index->Js.Int.toString;
    let name = baseName ++ suffix;
    LocalStorage.setItem("index", suffix);
    let path = derivationScheme->Js.String2.replace("?", suffix);
    HD.edesk(path, seed, ~password)
    ->Future.tapOk(edesk =>
        Js.log(baseName ++ index->Js.Int.toString ++ " " ++ edesk)
      )
    ->Future.flatMapOk(edesk =>
        import(edesk, name)
        ->Future.flatMapOk(_ =>
            get(~settings)
            ->Future.mapOk(MapString.fromArray)
            ->Future.flatMapOk(accounts =>
                switch (accounts->Map.String.get(name)) {
                | Some(address) =>
                  (
                    if (index == 0) {
                      // always include 0'
                      Future.value(Ok(true));
                    } else {
                      settings->validate(address);
                    }
                  )
                  ->Future.flatMapOk(isValidated =>
                      if (isValidated) {
                        scan(
                          ~settings,
                          seed,
                          baseName,
                          ~derivationScheme,
                          ~password,
                          ~index=index + 1,
                          (),
                        )
                        ->Future.mapOk(addresses =>
                            Array.concat([|address|], addresses)
                          );
                      } else {
                        delete(~settings, name)->Future.map(_ => Ok([||]));
                      }
                    )
                | None => Future.make(resolve => resolve(Ok([||])))
                }
              )
          )
      );
  };

  let restore =
      (
        ~settings,
        backupPhrase,
        name,
        ~derivationScheme="m/44'/1729'/?'/0'",
        ~password,
        (),
      ) => {
    scan(
      ~settings,
      backupPhrase->HD.seed,
      name,
      ~derivationScheme,
      ~password,
      (),
    )
    ->Future.tapOk(_ => {
        recoveryPhrases(~settings, password)
        ->Future.mapOk(recoveryPhrases =>
            recoveryPhrases->Option.mapWithDefault(
              [|backupPhrase|], recoveryPhrases =>
              Array.concat(recoveryPhrases, [|backupPhrase|])
            )
          )
        ->Future.mapOk(Json.Encode.(array(string)))
        ->Future.mapOk(json => json->Json.stringify)
        ->Future.flatMapOk(string =>
            string->SecureStorage.store(~key="recoveryPhrases", ~password)
          )
      })
    ->Future.tapOk(addresses => {
        Js.log(addresses);
        let secret = {
          Secret.name,
          derivationScheme,
          addresses,
          legacyAddress: None,
        };
        let secrets =
          secrets(~settings)
          ->Option.getWithDefault([||])
          ->Array.concat([|secret|]);
        LocalStorage.setItem(
          "secrets",
          Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
        );
      });
  };
};

module Aliases = (Caller: CallerAPI) => {
  let parse = content =>
    content
    |> Js.String.split("\n")
    |> Js.Array.map(row => row |> Js.String.split(": "))
    |> (pairs => pairs->Js.Array2.filter(pair => pair->Array.length == 2))
    |> Js.Array.map(pair =>
         (pair->Array.getUnsafe(0), pair->Array.getUnsafe(1))
       );

  let get = (~settings) =>
    Caller.call(
      [|"-E", settings->AppSettings.endpoint, "list", "known", "contracts"|],
      (),
    )
    ->Future.mapOk(parse);

  let getAliasForAddress = (~settings, address) =>
    get(~settings)
    ->Future.mapOk(addresses => addresses->Array.map(((a, b)) => (b, a)))
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(aliases => aliases->Map.String.get(address));

  let getAddressForAlias = (~settings, alias) =>
    get(~settings)
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Map.String.get(alias));

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let delete = (~settings, name) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "forget",
        "address",
        name,
        "-f",
      |],
      (),
    );
};

module Delegate = (Caller: CallerAPI, Getter: GetterAPI) => {
  let parse = content =>
    if (content == "none\n") {
      None;
    } else {
      let splittedContent = content->Js.String2.split(" ");
      if (content->Js.String2.length == 0 || splittedContent->Array.length == 0) {
        None;
      } else {
        Some(splittedContent->Array.getUnsafe(0));
      };
    };

  let getForAccount = (settings, account) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "get",
        "delegate",
        "for",
        account,
      |],
      (),
    )
    ->Future.mapOk(parse)
    ->Future.mapOk(result =>
        switch (result) {
        | Some(delegate) =>
          if (account == delegate) {
            None;
          } else {
            result;
          }
        | None => None
        }
      );

  let getBakers = (settings: AppSettings.t) =>
    switch (settings.network) {
    | Mainnet =>
      "https://api.baking-bad.org/v2/bakers"
      ->Getter.get
      ->Future.mapOk(Json.Decode.(array(Delegate.decode)))
    | Testnet =>
      Future.value(
        Ok([|
          {name: "zebra", address: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"},
        |]),
      )
    };

  type delegationInfo = {
    initialBalance: ProtocolXTZ.t,
    delegate: string,
    timestamp: Js.Date.t,
    lastReward: option(ProtocolXTZ.t),
  };

  let getDelegationInfoForAccount = (network, account: string) => {
    module OperationsAPI = Operations(Caller, Getter);
    module BalanceAPI = Balance(Caller);
    network
    ->OperationsAPI.get(account, ~types=[|"delegation"|], ~limit=1, ())
    ->Future.flatMapOk(operations =>
        if (operations->Array.length == 0) {
          //Future.value(Error("No delegation found!"));
          Future.value(
            Ok({
              initialBalance: ProtocolXTZ.zero,
              delegate: "",
              timestamp: Js.Date.make(),
              lastReward: None,
            }),
          );
        } else {
          let firstOperation = operations->Array.getUnsafe(0);
          switch (firstOperation.payload) {
          | Business(payload) =>
            switch (payload.payload) {
            | Delegation(payload) =>
              switch (payload.delegate) {
              | Some(delegate) =>
                if (account == delegate) {
                  //Future.value(Error("Bakers can't delegate!"));
                  Future.value(
                    Ok({
                      initialBalance: ProtocolXTZ.zero,
                      delegate: "",
                      timestamp: Js.Date.make(),
                      lastReward: None,
                    }),
                  );
                } else {
                  network
                  ->BalanceAPI.get(
                      account,
                      ~block=firstOperation.level->string_of_int,
                      (),
                    )
                  ->Future.mapOk(balance =>
                      {
                        initialBalance: balance,
                        delegate,
                        timestamp: firstOperation.timestamp,
                        lastReward: None,
                      }
                    )
                  ->Future.flatMapOk(info =>
                      network
                      ->OperationsAPI.get(
                          info.delegate,
                          ~types=[|"transaction"|],
                          ~destination=account,
                          ~limit=1,
                          (),
                        )
                      ->Future.mapOk(operations =>
                          if (operations->Array.length == 0) {
                            info;
                          } else {
                            switch (firstOperation.payload) {
                            | Business(payload) =>
                              switch (payload.payload) {
                              | Transaction(payload) => {
                                  ...info,
                                  lastReward: Some(payload.amount),
                                }
                              | _ => info
                              }
                            };
                          }
                        )
                    );
                }
              | None => Future.value(Error("No delegate set!"))
              }
            | _ => Future.value(Error("Invalid operation type!"))
            }
          };
        }
      );
  };
};

module Tokens = (Caller: CallerAPI, Getter: GetterAPI) => {
  module Injector = InjectorRaw(Caller);

  let getTokenViewer = settings => URL.tokenViewer(settings)->Getter.get;

  let checkTokenContract = (settings, addr) => {
    let arguments = [|
      "-E",
      settings->AppSettings.endpoint,
      "check",
      "contract",
      addr,
      "implements",
      "fungible",
      "assets",
    |];
    Caller.call(arguments, ());
  };

  let get = (settings: AppSettings.t) => {
    switch (settings.network) {
    | Testnet =>
      Future.value(
        Ok([|("Klondike", "KLD", "KT1BUdnCMfBKdVxCKyBvMUqwLqm27EDGWskB")|]),
      )
    | Mainnet => Future.value(Ok([||]))
    };
  };

  let transfer = (elt: Token.Transfer.elt) => {
    let obj = Js.Dict.empty();
    Js.Dict.set(obj, "destination", Js.Json.string(elt.destination));
    Js.Dict.set(obj, "amount", elt.amount->Js.Int.toString->Js.Json.string);
    Js.Dict.set(obj, "token", elt.token->Js.Json.string);
    elt.tx_options.fee
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "fee", v->ProtocolXTZ.toString->Js.Json.string)
      );
    elt.tx_options.gasLimit
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "gas-limit", Js.Int.toString(v)->Js.Json.string)
      );
    elt.tx_options.storageLimit
    ->Lib.Option.iter(v =>
        Js.Dict.set(obj, "storage-limit", Js.Int.toString(v)->Js.Json.string)
      );
    Js.Json.object_(obj);
  };

  let transfers_to_json = (transfers: list(Token.Transfer.elt)) =>
    transfers
    ->List.map(transfer)
    ->List.toArray
    ->Js.Json.array
    ->Js.Json.stringify /* ->(json => "\'" ++ json ++ "\'") */;

  let make_get_arguments =
      (arguments, callback, offline, tx_options, common_options) =>
    switch (callback, offline) {
    | (Some(callback), true) =>
      Js.Array2.concat(arguments, [|"offline", "with", callback|])
    | (Some(callback), false) =>
      Js.Array2.concat(arguments, [|"callback", "on", callback|])
      ->Injector.transaction_options_arguments(tx_options, common_options)
    | (None, _) => assert(false)
    };

  let make_arguments = (settings, operation: Token.operation, ~offline) => {
    switch (operation) {
    | Transfer({source, transfers: [elt], common_options}) =>
      [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "from",
        "token",
        "contract",
        elt.token,
        "transfer",
        Js.Int.toString(elt.amount),
        "from",
        source,
        "to",
        elt.destination,
        "--burn-cap",
        "0.01875",
      |]
      ->Injector.transaction_options_arguments(elt.tx_options, common_options)
    | Transfer({source, transfers, common_options}) =>
      [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "multiple",
        "tokens",
        "transfers",
        "from",
        source,
        "using",
        transfers_to_json(transfers),
        "--burn-cap",
        Js.Float.toString(0.08300 *. transfers->List.length->float_of_int),
      |]
      ->Injector.transaction_options_arguments(
          Protocol.emptyTransferOptions,
          common_options,
        )
    | GetBalance({
        token,
        address,
        callback,
        options: (tx_options, common_options),
      }) =>
      [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "from",
        "token",
        "contract",
        token,
        "get",
        "balance",
        "for",
        address,
      |]
      ->make_get_arguments(callback, offline, tx_options, common_options)
    | _ => assert(false)
    };
  };

  let offline = (operation: Token.operation) => {
    switch (operation) {
    | Transfer(_)
    | Approve(_) => false
    | GetBalance(_)
    | GetAllowance(_)
    | GetTotalSupply(_) => true
    };
  };

  let simulate = (network, operation: Token.operation) =>
    Injector.simulate(
      network,
      Injector.singleOperationParser,
      make_arguments(_, operation, ~offline=false),
    );

  let create = (network, operation: Token.operation) =>
    Injector.create(network, make_arguments(_, operation, ~offline=false));

  let inject = (network, operation: Token.operation, ~password) =>
    Injector.inject(
      network,
      make_arguments(_, operation, ~offline=false),
      ~password,
    );

  let callGetOperationOffline = (settings, operation: Token.operation) =>
    getTokenViewer(settings)
    ->Future.map(viewer => viewer->tryMap(Token.Decode.viewer))
    >>= (
      callback => {
        let operation = operation->Token.setCallback(callback);

        if (offline(operation)) {
          Caller.call(
            make_arguments(settings, operation, ~offline=true),
            (),
          );
        } else {
          Future.value(Error("Operation not runnable offline"));
        };
      }
    );
};
