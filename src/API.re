open ChildReprocess.StdStream;
open Common;
open Delegate;

module Path = {
  let delegates = "/chains/main/blocks/head/context/delegates\\?active=true";
  let operations = "operations";
  let mempool_operations = "mempool_operations";
};

module URL = {
  let arg_opt = (v, n, f) => v->Belt.Option.map(a => (n, f(a)));

  let build_args = l =>
    l->Belt.List.map(((a, v)) => a ++ "=" ++ v)->Belt.List.toArray
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

  let mempool = (network, account) =>
    build_url(network, Path.mempool_operations, [("pkh", account)]);

  let delegates = settings =>
    AppSettings.endpoint(settings) ++ Path.delegates;
};

module type CallerAPI = {
  let call:
    (array(string), ~inputs: array(string)=?, unit) =>
    Future.t(Belt.Result.t(string, string));
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
      let result: ref(option(Belt.Result.t(string, string))) = ref(None);
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
        ->Belt.Float.fromString
        ->Belt.Option.map(Belt.Float.toString)
        ->Belt.Option.flatMap(ProtocolXTZ.fromString)
        ->Lib.Result.fromOption("Cannot parse balance")
        ->Future.value
      );
  };
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
        Js.Re.exec_(regexp, receipt)->Belt.Option.map(Js.Re.captures),
        interp,
      ) {
      | (Some(res), Exists(_)) => [|res[0]->Js.Nullable.toOption|]
      | (Some(res), _) =>
        acc
        ->Js.Array2.concat([|res[1]->Js.Nullable.toOption|])
        ->parseAll(regexp)
      | (None, _) => acc
      };
    let interpretResults = results =>
      if (Js.Array.length(results) == 0) {
        None;
      } else {
        switch (interp) {
        | Exists(f) => results[0]->Belt.Option.flatMap(f)
        | First(f) => results[0]->Belt.Option.flatMap(f)
        | Last(f) =>
          results[Js.Array.length(results) - 1]->Belt.Option.flatMap(f)
        | Nth(f, i) =>
          i < Js.Array.length(results)
            ? results[i]->Belt.Option.flatMap(f) : None
        | AllReduce(f, init) => Some(Js.Array2.reduce(results, f, init))
        };
      };
    parseAll([||], Js.Re.fromStringWithFlags(pattern, ~flags="g"))
    ->interpretResults;
  };

  let parseAndReduceXTZ = (f, s) => {
    Belt.Option.mapWithDefault(
      s,
      Some(ProtocolXTZ.zero),
      ProtocolXTZ.fromString,
    )
    ->Belt.Option.mapWithDefault(f, ProtocolXTZ.Infix.(+)(f));
  };

  let parseAndReduceInt = (f, s) => {
    Belt.Option.mapWithDefault(s, Some(0), int_of_string_opt)
    ->Belt.Option.mapWithDefault(f, Belt.Int.(+)(f));
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
      switch (common_options.counter) {
      | Some(counter) =>
        Js.Array2.concat(arguments, [|"-C", counter->Js.Int.toString|])
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
      make_arguments(network)->Js.Array2.concat([|"-D"|]),
      ~inputs=
        switch (LocalStorage.getItem("password")->Js.Nullable.toOption) {
        | Some(password) => [|password|]
        | None => [||]
        },
      (),
    )
    ->Future.tapOk(Js.log)
    ->Future.map(result =>
        result->map(receipt => {
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
        result->map(receipt => {
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
        result->map(receipt => {
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
        result->map(x =>
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
          Belt.Set.fromArray(
            operations,
            ~id=(module Operation.Read.Comparator),
          );

        let operations =
          mempool
          ->Belt.Array.reduce(operations, Belt.Set.add)
          ->Belt.Set.toArray;

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
        result->map(Json.Decode.(array(Operation.Read.decode)))
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
    ->Belt.List.map(transfer)
    ->Belt.List.toArray
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
          0.06425 *. transaction.transfers->Belt.List.length->float_of_int,
        ),
      |]
    | Delegation(delegation) => [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "set",
        "delegate",
        "for",
        delegation.source,
        "to",
        delegation.delegate,
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

module MapString = Belt.Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module Accounts = (Caller: CallerAPI) => {
  let parse = content =>
    content
    ->Js.String2.split("\n")
    ->Belt.Array.map(row => row->Js.String2.split(":"))
    ->(rows => rows->Belt.Array.keep(data => data->Belt.Array.length >= 2))
    ->Belt.Array.map(pair => {
        [|pair[0]|]
        ->Belt.Array.concat(pair[1]->Js.String2.trim->Js.String2.split(" ("))
      })
    ->(rows => rows->Belt.Array.keep(data => data->Belt.Array.length > 2))
    ->Belt.Array.map(data => (data[0], data[1]));

  let get = (~settings: AppSettings.t) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapOk(r =>
        r->Belt.Array.keepMap(
          (TezosSDK.OutputAddress.{alias, pkh, sk_known}) =>
          sk_known ? Some((alias, pkh)) : None
        )
      );

  let create = (~settings, name) =>
    Caller.call(
      [|"-E", settings->AppSettings.endpoint, "gen", "keys", name|],
      (),
    );

  let add = (settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let import = (key, name) =>
    Caller.call(
      [|"import", "secret", "key", name, "unencrypted:" ++ key|],
      (),
    );

  let addWithMnemonic = (~settings, name, mnemonic, ~password) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "import",
        "keys",
        "from",
        "mnemonic",
        name,
        "--encrypt",
      |],
      ~inputs=[|mnemonic, "", password, password|],
      (),
    )
    ->Future.tapOk(_ => {
        LocalStorage.setItem("mnemonic", mnemonic);
        LocalStorage.setItem("password", password);
      });

  let restore = (~settings, backupPhrase, name, ~derivationPath=?, ()) => {
    switch (derivationPath) {
    | Some(derivationPath) =>
      let seed = HD.BIP39.mnemonicToSeedSync(backupPhrase);
      let edsk2 = HD.seedToPrivateKey(HD.deriveSeed(seed, derivationPath));
      Js.log(edsk2);
      import(edsk2, name);
    | None =>
      Caller.call(
        [|
          "-E",
          settings->AppSettings.endpoint,
          "generate",
          "keys",
          "from",
          "mnemonic",
          backupPhrase,
        |],
        (),
      )
      ->Future.tapOk(Js.log)
      ->Future.mapOk(keys => (keys |> Js.String.split("\n"))[2])
      ->Future.tapOk(Js.log)
      ->Future.flatMapOk(edsk => import(edsk, name))
    };
  };

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

  let delegate = (settings, account, delegate) =>
    Caller.call(
      [|
        "-E",
        settings->AppSettings.endpoint,
        "set",
        "delegate",
        "for",
        account,
        "to",
        delegate,
      |],
      (),
    );
};

module Scanner = (Caller: CallerAPI, Getter: GetterAPI) => {
  module AccountsAPI = Accounts(Caller);
  module OperationsAPI = Operations(Caller, Getter);

  let validate = (network, address) => {
    network
    ->OperationsAPI.get(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0});
  };

  let rec scan =
          (
            settings: AppSettings.t,
            backupPhrase,
            baseName,
            ~derivationSchema,
            ~index,
          ) => {
    let seed = HD.BIP39.mnemonicToSeedSync(backupPhrase);
    let suffix = index->Js.Int.toString;
    LocalStorage.setItem("index", suffix);
    let derivationPath = derivationSchema->Js.String2.replace("?", suffix);
    let edsk2 = HD.seedToPrivateKey(HD.deriveSeed(seed, derivationPath));
    Js.log(baseName ++ index->Js.Int.toString ++ " " ++ edsk2);
    let name = baseName ++ suffix;
    AccountsAPI.import(edsk2, name)
    ->Future.flatMapOk(_ =>
        AccountsAPI.get(~settings)
        ->Future.mapOk(MapString.fromArray)
        ->Future.flatMapOk(accounts =>
            switch (accounts->Belt.Map.String.get(name)) {
            | Some(address) =>
              settings
              ->validate(address)
              ->Future.flatMapOk(isValidated =>
                  if (isValidated) {
                    scan(
                      settings,
                      backupPhrase,
                      baseName,
                      ~derivationSchema,
                      ~index=index + 1,
                    );
                  } else {
                    AccountsAPI.delete(~settings, name);
                  }
                )
            | None => Future.make(resolve => resolve(Ok("")))
            }
          )
      );
  };
};

module Aliases = (Caller: CallerAPI) => {
  let parse = content =>
    content
    |> Js.String.split("\n")
    |> Js.Array.map(row => row |> Js.String.split(": "))
    |> (pairs => pairs->Js.Array2.filter(pair => pair->Array.length == 2))
    |> Js.Array.map(pair => (pair[0], pair[1]))
    |> Js.Array.sortInPlaceWith((a, b) => {
         let (a, _) = a;
         let (b, _) = b;
         a->compare(b);
       });

  let get = (~settings) =>
    Caller.call(
      [|"-E", settings->AppSettings.endpoint, "list", "known", "contracts"|],
      (),
    )
    ->Future.mapOk(parse);

  let getAliasForAddress = (~settings, address) =>
    get(~settings)
    ->Future.mapOk(addresses =>
        addresses->Belt.Array.map(((a, b)) => (b, a))
      )
    ->Future.mapOk(Belt.Map.String.fromArray)
    ->Future.mapOk(aliases => aliases->Belt.Map.String.get(address));

  let getAddressForAlias = (~settings, alias) =>
    get(~settings)
    ->Future.mapOk(Belt.Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Belt.Map.String.get(alias));

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
      if (content->Js.String2.length == 0
          || splittedContent->Belt.Array.length == 0) {
        None;
      } else {
        Some(splittedContent[0]);
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
      ->Future.map(result => result->map(Json.Decode.(array(decode))))
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
        if (operations->Belt.Array.length == 0) {
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
          switch (operations[0].payload) {
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
                  ->BalanceAPI.get(account, ~block=operations[0].level, ())
                  ->Future.mapOk(balance =>
                      {
                        initialBalance: balance,
                        delegate,
                        timestamp: operations[0].timestamp,
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
                          if (operations->Belt.Array.length == 0) {
                            info;
                          } else {
                            switch (operations[0].payload) {
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

module Tokens = (Caller: CallerAPI) => {
  module Injector = InjectorRaw(Caller);

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

  let make_get_arguments =
      (arguments, callback, offline, tx_options, common_options) =>
    if (offline) {
      Js.Array2.concat(arguments, [|"offline", "with", callback|]);
    } else {
      Js.Array2.concat(arguments, [|"callback", "on", callback|])
      ->Injector.transaction_options_arguments(tx_options, common_options);
    };

  let make_arguments = (settings, operation: Token.operation, ~offline) => {
    switch (operation.action) {
    | Transfer(transfer) =>
      [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "from",
        "token",
        "contract",
        operation.token,
        "transfer",
        Js.Int.toString(transfer.amount),
        "from",
        transfer.source,
        "to",
        transfer.destination,
        "--burn-cap",
        "0.01875",
      |]
      ->Injector.transaction_options_arguments(
          operation.tx_options,
          operation.common_options,
        )
    | GetBalance(getBalance) =>
      [|
        "-E",
        settings->AppSettings.endpoint,
        "-w",
        "none",
        "from",
        "token",
        "contract",
        operation.token,
        "get",
        "balance",
        "for",
        getBalance.address,
      |]
      ->make_get_arguments(
          getBalance.callback,
          offline,
          operation.tx_options,
          operation.common_options,
        )
    | _ => assert(false)
    };
  };

  let offline = (operation: Token.operation) => {
    switch (operation.action) {
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

  let callGetOperationOffline = (network, operation: Token.operation) =>
    if (offline(operation)) {
      Caller.call(make_arguments(network, operation, ~offline=true), ());
    } else {
      Future.value(Error("Operation not runnable offline"));
    };
};
