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
      let result = ref(Result.Ok(""));
      let _ =
        process
        ->child_stderr
        ->Readable.on_data(buffer => {
            let error = Node_buffer.toString(buffer);
            if (removeTestnetWarning(error) != "") {
              result := Error(error);
            };
          });
      let _ =
        process
        ->child_stdout
        ->Readable.on_data(buffer =>
            switch (result^) {
            | Ok(text) =>
              result :=
                Ok(Js.String2.concat(text, buffer->Node_buffer.toString))
            | Error(_) => ()
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
      let _ = process->ChildReprocess.on_close((_, _) => resolve(result^));
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

module Balance = {
  let get = (settings, address, ~params=?, ()) => {
    AppSettings.endpoint(settings)
    ->ReTaquito.Balance.get(~address, ~params?, ())
    ->Future.mapOk(ProtocolXTZ.ofInt64);
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
          switch (fee, gasLimit, storageLimit) {
          | (Some(fee), Some(gasLimit), Some(storageLimit)) =>
            Protocol.{fee, gasLimit, storageLimit}
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

  let transferEstimate = (settings, transfer, source) => {
    ReTaquito.Estimate.transfer(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~dest=transfer.Protocol.destination,
      ~amount=transfer.Protocol.amount->ProtocolXTZ.toInt64,
      ~fee=?transfer.Protocol.tx_options.fee->Option.map(ProtocolXTZ.toInt64),
      ~gasLimit=?transfer.Protocol.tx_options.gasLimit,
      ~storageLimit=?transfer.Protocol.tx_options.storageLimit,
      (),
    );
  };

  let batchEstimate = (settings, transfers, ~source, ~index=?, ()) => {
    let transfers = source =>
      transfers
      ->List.map(({amount, destination, tx_options}: Protocol.transfer) =>
          ReTaquito.Toolkit.prepareTransfer(
            ~amount=amount->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64,
            ~dest=destination,
            ~source,
            ~fee=?
              tx_options.fee
              ->Option.map(fee =>
                  fee->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64
                ),
            ~gasLimit=?tx_options.gasLimit,
            ~storageLimit=?tx_options.storageLimit,
            (),
          )
        )
      ->List.toArray;

    ReTaquito.Estimate.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      (),
    )
    ->Future.flatMapOk(r =>
        ReTaquito.Estimate.handleEstimationResults(r, index)
      );
  };

  let setDelegateEstimate = (settings, delegation: Protocol.delegation) => {
    ReTaquito.Estimate.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source=delegation.Protocol.source,
      ~delegate=?delegation.Protocol.delegate,
      ~fee=?delegation.Protocol.options.fee->Option.map(ProtocolXTZ.toInt64),
      (),
    );
  };

  let simulate = (settings, ~index=?, operation: Protocol.t) => {
    let r =
      switch (operation, index) {
      | (Transaction({transfers: [t], source}), _) =>
        transferEstimate(settings, t, source)

      | (Delegation(d), _) => setDelegateEstimate(settings, d)
      | (Transaction({transfers, source}), None) =>
        batchEstimate(settings, transfers, ~source, ())

      | (Transaction({transfers, source}), Some(index)) =>
        batchEstimate(settings, transfers, ~source, ~index, ())
      };

    r
    ->Future.mapError(e =>
        switch (e) {
        | Generic(s) => s
        | WrongPassword => I18n.form_input_error#wrong_password
        }
      )
    ->Future.mapOk(({totalCost, gasLimit, storageLimit}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
        }
      );
  };

  let create = (network, operation: Protocol.t) =>
    Injector.create(network, arguments(_, operation));

  let injectBatch = (settings, transfers, ~source, ~password) => {
    let transfers = source =>
      transfers
      ->List.map(({amount, destination, tx_options}: Protocol.transfer) =>
          ReTaquito.Toolkit.prepareTransfer(
            ~amount=amount->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64,
            ~dest=destination,
            ~source,
            ~fee=?
              tx_options.fee
              ->Option.map(fee =>
                  fee->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64
                ),
            ~gasLimit=?tx_options.gasLimit,
            ~storageLimit=?tx_options.storageLimit,
            (),
          )
        )
      ->List.toArray;

    ReTaquito.Operations.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      ~password,
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let injectTransfer = (settings, transfer, ~source, ~password) => {
    ReTaquito.Operations.transfer(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~dest=transfer.Protocol.destination,
      ~amount=transfer.Protocol.amount->ProtocolXTZ.toInt64,
      ~password,
      ~fee=?transfer.Protocol.tx_options.fee->Option.map(ProtocolXTZ.toInt64),
      ~gasLimit=?transfer.Protocol.tx_options.gasLimit,
      ~storageLimit=?transfer.Protocol.tx_options.storageLimit,
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let injectSetDelegate =
      (settings, Protocol.{delegate, source, options}, ~password) => {
    ReTaquito.Operations.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~delegate,
      ~password,
      ~fee=?options.fee->Option.map(ProtocolXTZ.toInt64),
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let inject = (settings, operation: Protocol.t, ~password) =>
    switch (operation) {
    | Transaction({transfers: [t], source}) =>
      injectTransfer(settings, t, ~source, ~password)

    | Delegation(d) => injectSetDelegate(settings, d, ~password)

    | Transaction({transfers, source}) =>
      injectBatch(settings, transfers, ~source, ~password)
    };

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
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapOk(l => l->Array.map(({alias, pkh}) => (alias, pkh)));

  let getAliasMap = (~settings) =>
    get(~settings)
    ->Future.mapOk(addresses => addresses->Array.map(((a, b)) => (b, a)))
    ->Future.mapOk(Map.String.fromArray);

  let getAliasForAddress = (~settings, address) =>
    getAliasMap(~settings)
    ->Future.mapOk(aliases => aliases->Map.String.get(address));

  let getAddressForAlias = (~settings, alias) =>
    get(~settings)
    ->Future.mapOk(Map.String.fromArray)
    ->Future.mapOk(addresses => addresses->Map.String.get(alias));

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let delete = (~settings, name) =>
    settings->AppSettings.sdk->TezosSDK.forgetAddress(name);
};

module Accounts = (Caller: CallerAPI, Getter: GetterAPI) => {
  let secrets = (~settings: AppSettings.t) => {
    let _ = settings;
    LocalStorage.getItem("secrets")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(Secret.decoder)));
  };

  let recoveryPhrases = (~settings: AppSettings.t) => {
    let _ = settings;
    LocalStorage.getItem("recovery-phrases")
    ->Js.Nullable.toOption
    ->Option.flatMap(Json.parse)
    ->Option.map(Json.Decode.(array(SecureStorage.Cipher.decoder)));
  };

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

  let get = (~settings: AppSettings.t) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.listKnownAddresses
    ->Future.mapOk(r =>
        r->Array.keepMap((TezosSDK.OutputAddress.{alias, pkh, sk_known}) =>
          sk_known ? Some((alias, pkh)) : None
        )
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
    recoveryPhrases(~settings)
    ->Option.flatMap(recoveryPhrases => recoveryPhrases[index])
    ->FutureEx.fromOption(
        ~error=
          "Recovery phrase at index " ++ index->Int.toString ++ " not found!",
      )
    ->Future.flatMapOk(SecureStorage.Cipher.decrypt2(password));

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let import = (~settings, key, name, ~password) => {
    let skUri = "encrypted:" ++ key;
    settings
    ->AppSettings.sdk
    ->TezosSDK.importSecretKey(~name, ~skUri, ~password, ())
    ->Future.tapOk(k => Js.log("key found : " ++ k));
  };

  let derive = (~settings, ~index, ~name, ~password) =>
    Future.mapOk2(
      secretAt(~settings, index),
      recoveryPhraseAt(~settings, index, ~password),
      (secret, recoveryPhrase) => {
        let suffix = secret.addresses->Array.length->Js.Int.toString;
        let path = secret.derivationScheme->Js.String2.replace("?", suffix);
        HD.edesk(path, recoveryPhrase->HD.seed, ~password)
        ->Future.flatMapOk(edesk => import(~settings, edesk, name, ~password))
        ->Future.tapOk(address =>
            {
              ...secret,
              addresses: Array.concat(secret.addresses, [|address|]),
            }
            ->updateSecretAt(~settings, index)
          );
      },
    )
    ->Future.flatMapOk(update => update);

  module AliasesAPI = Aliases(Caller);

  let unsafeDelete = (~settings, name) =>
    settings->AppSettings.sdk->TezosSDK.forgetAddress(name);

  let delete = (~settings, name) =>
    AliasesAPI.getAddressForAlias(~settings, name)
    ->Future.flatMapOk(address =>
        unsafeDelete(~settings, name)
        ->Future.mapOk(_ =>
            secrets(~settings)
            ->Option.map(secrets =>
                secrets->Array.map(secret =>
                  address == secret.legacyAddress
                    ? {...secret, legacyAddress: None} : secret
                )
              )
            ->Option.map(secrets =>
                Json.Encode.array(Secret.encoder, secrets)->Json.stringify
              )
            ->Option.map("secrets"->LocalStorage.setItem)
          )
      );

  let deleteSecretAt = (~settings, index) =>
    Future.mapOk2(
      secrets(~settings)->FutureEx.fromOption(~error="No secrets found!"),
      AliasesAPI.getAliasMap(~settings),
      (secrets, aliases) => {
      secrets[index]
      ->Option.map(secret =>
          secret.addresses
          ->Array.keepMap(aliases->Map.String.get)
          ->Array.map(unsafeDelete(~settings))
        )
      ->FutureEx.fromOption(
          ~error="Secret at index " ++ index->Int.toString ++ " not found!",
        )
      ->Future.flatMapOk(FutureEx.all)
      ->Future.tapOk(_ => {
          let _ =
            secrets->Js.Array2.spliceInPlace(
              ~pos=index,
              ~remove=1,
              ~add=[||],
            );
          LocalStorage.setItem(
            "secrets",
            Json.Encode.array(Secret.encoder, secrets)->Json.stringify,
          );
        })
      ->Future.tapOk(_ =>
          switch (recoveryPhrases(~settings)) {
          | Some(recoveryPhrases) =>
            let _ =
              recoveryPhrases->Js.Array2.spliceInPlace(
                ~pos=index,
                ~remove=1,
                ~add=[||],
              );
            LocalStorage.setItem(
              "recovery-phrases",
              Json.Encode.array(SecureStorage.Cipher.encoder, recoveryPhrases)
              ->Json.stringify,
            );
          | None => ()
          }
        )
    })
    ->Future.flatMapOk(update => update)
    ->Future.tapOk(_ => {
        let recoveryPhrases = recoveryPhrases(~settings);
        if (recoveryPhrases == Some([||]) || recoveryPhrases == None) {
          "lock"->LocalStorage.removeItem;
          "recovery-phrases"->LocalStorage.removeItem;
          "secrets"->LocalStorage.removeItem;
        };
      });

  let validate = (network, address) => {
    module OperationsAPI = Operations(Caller, Getter);
    network
    ->OperationsAPI.get(address, ~limit=1, ())
    ->Future.mapOk(operations => {operations->Js.Array2.length != 0});
  };

  let rec scanSeed =
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
    let name = baseName ++ " /" ++ suffix;
    let path = derivationScheme->Js.String2.replace("?", suffix);
    HD.edesk(path, seed, ~password)
    ->Future.flatMapOk(edesk =>
        import(~settings, edesk, name, ~password)
        ->Future.flatMapOk(address
            // always include 0'
            =>
              (
                index == 0
                  ? Future.value(Ok(true)) : settings->validate(address)
              )
              ->Future.flatMapOk(isValidated =>
                  isValidated
                    ? scanSeed(
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
                        )
                    : unsafeDelete(~settings, name)->Future.map(_ => Ok([||]))
                )
            )
      );
  };

  let legacyImport = (~settings, name, recoveryPhrase, ~password) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.importKeysFromMnemonics(
        ~name,
        ~mnemonics=recoveryPhrase,
        ~password,
        (),
      );

  let legacyScan = (~settings, recoveryPhrase, name, ~password) =>
    legacyImport(~settings, name, recoveryPhrase, ~password)
    ->Future.flatMapOk(legacyAddress =>
        settings
        ->validate(legacyAddress)
        ->Future.mapOk(isValidated =>
            isValidated ? Some(legacyAddress) : None
          )
      )
    ->Future.flatMapOk(legacyAddress =>
        legacyAddress == None
          ? unsafeDelete(~settings, name)->Future.map(_ => Ok(None))
          : Future.value(Ok(legacyAddress))
      );

  let scan =
      (
        ~settings,
        recoveryPhrase,
        baseName,
        ~derivationScheme="m/44'/1729'/?'/0'",
        ~password,
        ~index=0,
        (),
      ) =>
    scanSeed(
      ~settings,
      recoveryPhrase->HD.seed,
      baseName,
      ~derivationScheme,
      ~password,
      ~index,
      (),
    )
    ->Future.flatMapOk(addresses => {
        legacyScan(
          ~settings,
          recoveryPhrase,
          baseName ++ " legacy",
          ~password,
        )
        ->Future.mapOk(legacyAddresses => (addresses, legacyAddresses))
      });

  let restore =
      (
        ~settings,
        backupPhrase,
        name,
        ~derivationScheme="m/44'/1729'/?'/0'",
        ~password,
        (),
      ) => {
    password
    ->SecureStorage.validatePassword
    ->Future.flatMapOk(_ =>
        scan(~settings, backupPhrase, name, ~derivationScheme, ~password, ())
      )
    ->Future.tapOk(_ =>
        Future.mapOk2(
          recoveryPhrases(~settings)
          ->FutureEx.fromOptionWithDefault(~default=[||]),
          backupPhrase->SecureStorage.Cipher.encrypt(password),
          (recoveryPhrases, newRecoveryPhrase) => {
          Array.concat(recoveryPhrases, [|newRecoveryPhrase|])
        })
        ->Future.mapOk(Json.Encode.(array(SecureStorage.Cipher.encoder)))
        ->Future.mapOk(json => json->Json.stringify)
        ->FutureEx.getOk(recoveryPhrases =>
            LocalStorage.setItem("recovery-phrases", recoveryPhrases)
          )
      )
    ->Future.tapOk(((addresses, legacyAddress)) => {
        let secret = {
          Secret.name,
          derivationScheme,
          addresses,
          legacyAddress,
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

  let unsafeDeleteAddresses = (~settings, addresses) =>
    AliasesAPI.getAliasMap(~settings)
    ->Future.mapOk(aliases =>
        addresses->Array.keepMap(aliases->Map.String.get)
      )
    ->Future.flatMapOk(names =>
        names->Array.map(unsafeDelete(~settings))->FutureEx.all
      );

  let scanAll = (~settings, ~password) =>
    (
      switch (recoveryPhrases(~settings), secrets(~settings)) {
      | (Some(recoveryPhrases), Some(secrets)) =>
        Array.zip(recoveryPhrases, secrets)
        ->Array.map(((recoveryPhrase, secret)) =>
            recoveryPhrase
            ->SecureStorage.Cipher.decrypt(password)
            ->Future.flatMapOk(recoveryPhrase =>
                scan(
                  ~settings,
                  recoveryPhrase,
                  secret.name,
                  ~derivationScheme=secret.derivationScheme,
                  ~password,
                  ~index=secret.addresses->Array.length,
                  (),
                )
              )
            ->Future.mapOk(((addresses, legacyAddress)) =>
                {
                  ...secret,
                  addresses: secret.addresses->Array.concat(addresses),
                  legacyAddress,
                }
              )
            ->Future.flatMapError(_ => Future.value(Ok(secret)))
          )
      | _ => [||]
      }
    )
    ->FutureEx.all
    ->Future.mapOk(secrets =>
        secrets->Array.keepMap(secret =>
          switch (secret) {
          | Ok(secret) => Some(secret)
          | _ => None
          }
        )
      )
    ->Future.mapOk(secrets =>
        Json.Encode.array(Secret.encoder, secrets)->Json.stringify
      )
    ->Future.mapOk(LocalStorage.setItem("secrets"));
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
    ReTaquito.getDelegate(settings->AppSettings.endpoint, account)
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
    module BalanceAPI = Balance;
    network
    ->OperationsAPI.get(account, ~types=[|"delegation"|], ~limit=1, ())
    ->Future.flatMapOk(operations =>
        if (operations->Array.length == 0) {
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
                      ~params={block: firstOperation.level->string_of_int},
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

  let injectBatch = (settings, transfers, ~source, ~password) => {
    let transfers = source =>
      transfers
      ->List.map(
          ({token, amount, destination, tx_options}: Token.Transfer.elt) =>
          ReTaquito.FA12Operations.toRawTransfer(
            ~token,
            ~amount=amount->ReTaquito.BigNumber.fromInt,
            ~dest=destination,
            ~fee=?
              tx_options.fee
              ->Option.map(fee =>
                  fee->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64
                ),
            ~gasLimit=?tx_options.gasLimit,
            ~storageLimit=?tx_options.storageLimit,
            (),
          )
        )
      ->ReTaquito.FA12Operations.prepareTransfers(
          source,
          settings->AppSettings.endpoint,
        );

    ReTaquito.FA12Operations.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      ~password,
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

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

  let transferEstimate = (settings, transfer, source) => {
    ReTaquito.FA12Operations.Estimate.transfer(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~tokenContract=transfer.Token.Transfer.token,
      ~source,
      ~dest=transfer.Token.Transfer.destination,
      ~amount=transfer.Token.Transfer.amount->Int64.of_int,
      ~fee=?
        transfer.Token.Transfer.tx_options.fee
        ->Option.map(ProtocolXTZ.toInt64),
      ~gasLimit=?transfer.Token.Transfer.tx_options.gasLimit,
      ~storageLimit=?transfer.Token.Transfer.tx_options.storageLimit,
      (),
    )
    ->Future.mapOk(({totalCost, gasLimit, storageLimit}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
        }
      );
  };

  let batchEstimate = (settings, transfers, ~source, ~index=?, ()) => {
    let transfers = source =>
      transfers
      ->List.map(
          ({token, amount, destination, tx_options}: Token.Transfer.elt) =>
          ReTaquito.FA12Operations.toRawTransfer(
            ~token,
            ~amount=amount->ReTaquito.BigNumber.fromInt,
            ~dest=destination,
            ~fee=?
              tx_options.fee
              ->Option.map(fee =>
                  fee->ProtocolXTZ.toInt64->ReTaquito.BigNumber.fromInt64
                ),
            ~gasLimit=?tx_options.gasLimit,
            ~storageLimit=?tx_options.storageLimit,
            (),
          )
        )
      ->ReTaquito.FA12Operations.prepareTransfers(
          source,
          settings->AppSettings.endpoint,
        );

    ReTaquito.FA12Operations.Estimate.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      (),
    )
    ->Future.flatMapOk(r =>
        ReTaquito.Estimate.handleEstimationResults(r, index)
      )
    ->Future.mapOk(({totalCost, gasLimit, storageLimit}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
        }
      );
  };

  let simulate = (network, operation: Token.operation) =>
    switch (operation) {
    | Transfer({source, transfers: [elt], _}) =>
      transferEstimate(network, elt, source)
      ->Future.mapError(e =>
          switch (e) {
          | Generic(s) => s
          | WrongPassword => I18n.form_input_error#wrong_password
          }
        )
    | Transfer({source, transfers, _}) =>
      batchEstimate(network, transfers, ~source, ())
      ->Future.mapError(e =>
          switch (e) {
          | Generic(s) => s
          | WrongPassword => I18n.form_input_error#wrong_password
          }
        )
    | _ =>
      Injector.simulate(
        network,
        Injector.singleOperationParser,
        make_arguments(_, operation, ~offline=false),
      )
    };

  let create = (network, operation: Token.operation) =>
    Injector.create(network, make_arguments(_, operation, ~offline=false));

  let transfer = (settings, transfer, source, password) => {
    ReTaquito.FA12Operations.transfer(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~tokenContract=transfer.Token.Transfer.token,
      ~source,
      ~dest=transfer.Token.Transfer.destination,
      ~amount=transfer.Token.Transfer.amount->Int64.of_int,
      ~password,
      ~fee=?
        transfer.Token.Transfer.tx_options.fee
        ->Option.map(ProtocolXTZ.toInt64),
      ~gasLimit=?transfer.Token.Transfer.tx_options.gasLimit,
      ~storageLimit=?transfer.Token.Transfer.tx_options.storageLimit,
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let inject = (network, operation: Token.operation, ~password) =>
    switch (operation) {
    | Transfer({source, transfers: [elt], _}) =>
      transfer(network, elt, source, password)
    | Transfer({source, transfers, _}) =>
      injectBatch(network, transfers, ~source, ~password)
    | _ =>
      Injector.inject(
        network,
        make_arguments(_, operation, ~offline=false),
        ~password,
      )
      ->Future.map(v =>
          switch (v) {
          | Ok((hash, _)) => Ok(hash)
          | Error(e) => Error(ReTaquito.Generic(e))
          }
        )
    };

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
