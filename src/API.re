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

  let checkToken = (network, contract) => {
    let path = "tokens/exists/" ++ contract;
    build_url(network, path, []);
  };

  let getTokenBalance = (network, contract, addr) => {
    let path = "accounts/" ++ addr ++ "/tokens/" ++ contract ++ "/balance";
    build_url(network, path, []);
  };
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

module Explorer = (Getter: GetterAPI) => {
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
};

let handleTaquitoError = e =>
  e->ReTaquito.Error.(
       fun
       | Generic(s) => s
       | WrongPassword => I18n.form_input_error#wrong_password
       | UnregisteredDelegate => I18n.form_input_error#unregistered_delegate
       | UnchangedDelegate => I18n.form_input_error#change_baker
       | BadPkh => I18n.form_input_error#bad_pkh
       | BranchRefused => I18n.form_input_error#branch_refused_error
       | InvalidContract => I18n.form_input_error#invalid_contract
     );

let handleSdkError = e =>
  e->TezosSDK.Error.(
       fun
       | Generic(s) => s
       | BadPkh => I18n.form_input_error#bad_pkh
     );

module CSV = {
  open CSVParser;

  type error =
    | Parser(CSVParser.error)
    | NoRows
    | CannotMixTokens(int)
    | CannotParseTokenAmount(ReBigNumber.t, int, int)
    | CannotParseTezAmount(ReBigNumber.t, int, int);

  type t =
    | TezRows(list(Protocol.transfer))
    | TokenRows(list(Token.Transfer.elt));

  let addr = Encodings.string;
  let token = addr;

  let rowEncoding =
    Encodings.(mkRow(tup4(addr, number, opt(token), opt(number))));

  let checkTokenUnique = rows =>
    rows
    ->List.reduceWithIndex(Ok(None), (prev, (_, _, token, _), index) =>
        switch (prev) {
        | Error(e) => Error(e)
        | Ok(Some(t)) => t == token ? prev : Error(CannotMixTokens(index))
        | Ok(None) => Ok(Some(token))
        }
      )
    ->Result.map(_ => rows);

  let handleTezCSV = res =>
    res->Result.flatMap(rows =>
      rows
      ->List.mapWithIndex((index, (destination, amount, _, _)) =>
          amount
          ->ReBigNumber.toString
          ->ProtocolXTZ.fromString
          ->ResultEx.fromOption(
              Error(CannotParseTezAmount(amount, index, 2)),
            )
          ->Result.map(amount =>
              Protocol.makeTransfer(~destination, ~amount, ())
            )
        )
      ->ResultEx.collect
    );

  let handleTokenCSV = (res, token) =>
    res->Result.flatMap(rows =>
      rows
      ->List.reverse
      ->List.reduceWithIndex(
          [],
          (acc, (destination, amount, _, _), index) => {
            let tx =
              amount
              ->Token.Repr.fromBigNumber
              ->ResultEx.fromOption(
                  Error(CannotParseTokenAmount(amount, index, 2)),
                )
              ->Result.map(amount =>
                  Token.makeSingleTransferElt(
                    ~destination,
                    ~amount,
                    ~token,
                    (),
                  )
                );
            [tx, ...acc];
          },
        )
      ->ResultEx.collect
    );

  let parseCSV = content => {
    let rows =
      parseCSV(content, rowEncoding)
      ->ResultEx.mapError(e => Error(Parser(e)))
      ->Result.flatMap(checkTokenUnique);
    switch (rows) {
    | Ok([(_, _, None, _), ..._]) =>
      rows->handleTezCSV->Result.map(r => TezRows(r))
    | Ok([(_, _, Some(token), _), ..._]) =>
      handleTokenCSV(rows, token)->Result.map(r => TokenRows(r))
    | Ok([]) => Error(NoRows)
    | Error(e) => Error(e)
    };
  };
};

let handleCSVError = e =>
  e->CSVParser.(
       fun
       | CSV.Parser(CannotParseNumber(row, col)) =>
         I18n.csv#cannot_parse_number(row + 1, col + 1)
       | Parser(CannotParseBool(row, col)) =>
         I18n.csv#cannot_parse_boolean(row + 1, col + 1)
       | Parser(CannotParseCustomValue(row, col)) =>
         I18n.csv#cannot_parse_custom_value(row + 1, col + 1)
       | Parser(CannotParseRow(row)) => I18n.csv#cannot_parse_row(row + 1)
       | Parser(CannotParseCSV) => I18n.csv#cannot_parse_csv
       | NoRows => I18n.csv#no_rows
       | CannotMixTokens(row) => I18n.csv#cannot_mix_tokens(row + 1)
       | CannotParseTokenAmount(v, row, col) =>
         I18n.csv#cannot_parse_token_amount(v, row + 1, col + 1)
       | CannotParseTezAmount(v, row, col) =>
         I18n.csv#cannot_parse_tez_amount(v, row + 1, col + 1)
     );

module Simulation = {
  let extractCustomValues = (tx_options: Protocol.transfer_options) => (
    tx_options.Protocol.fee
    ->Option.map(fee => fee->ProtocolXTZ.unsafeToMutezInt),
    tx_options.storageLimit,
    tx_options.gasLimit,
  );

  let transfer = (settings, transfer, source) => {
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

  let batch = (settings, transfers, ~source, ~index=?, ()) => {
    let customValues =
      List.map(transfers, tx => tx.Protocol.tx_options->extractCustomValues)
      ->List.toArray;

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
        ReTaquito.Estimate.handleEstimationResults(r, customValues, index)
      );
  };

  let setDelegate = (settings, delegation: Protocol.delegation) => {
    ReTaquito.Estimate.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source=delegation.Protocol.source,
      ~delegate=?delegation.Protocol.delegate,
      ~fee=?delegation.Protocol.options.fee->Option.map(ProtocolXTZ.toInt64),
      (),
    );
  };

  let run = (settings, ~index=?, operation: Protocol.t) => {
    let r =
      switch (operation, index) {
      | (Transaction({transfers: [t], source}), _) =>
        transfer(settings, t, source)
      | (Delegation(d), _) => setDelegate(settings, d)
      | (Transaction({transfers, source}), None) =>
        batch(settings, transfers, ~source, ())
      | (Transaction({transfers, source}), Some(index)) =>
        batch(settings, transfers, ~source, ~index, ())
      };

    r
    ->Future.mapError(e => e->handleTaquitoError)
    ->Future.mapOk(({totalCost, gasLimit, storageLimit, revealFee}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->ProtocolXTZ.fromMutezInt,
        }
      );
  };
};

module Operation = {
  let batch = (settings, transfers, ~source, ~password) => {
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

  let transfer = (settings, transfer, ~source, ~password) => {
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

  let setDelegate =
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

  let run = (settings, operation: Protocol.t, ~password) =>
    switch (operation) {
    | Transaction({transfers: [t], source}) =>
      transfer(settings, t, ~source, ~password)

    | Delegation(d) => setDelegate(settings, d, ~password)

    | Transaction({transfers, source}) =>
      batch(settings, transfers, ~source, ~password)
    };
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

module Aliases = {
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
    ->Future.mapError(handleSdkError)
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
    settings
    ->AppSettings.sdk
    ->TezosSDK.addAddress(alias, pkh)
    ->Future.mapError(handleSdkError);

  let delete = (~settings, name) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.forgetAddress(name)
    ->Future.mapError(handleSdkError);

  let rename = (~settings, renaming) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.renameAliases(renaming)
    ->Future.mapError(handleSdkError);
};

module Accounts = (Getter: GetterAPI) => {
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
    ->Future.mapError(handleSdkError)
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
    ->Future.flatMapOk(SecureStorage.Cipher.decrypt2(password))
    ->Future.mapError(_ => {I18n.form_input_error#wrong_password});

  let add = (~settings, alias, pkh) =>
    settings->AppSettings.sdk->TezosSDK.addAddress(alias, pkh);

  let import = (~settings, key, name, ~password) => {
    let skUri = "encrypted:" ++ key;
    settings
    ->AppSettings.sdk
    ->TezosSDK.importSecretKey(~name, ~skUri, ~password, ())
    ->Future.mapError(handleSdkError)
    ->Future.tapOk(k => Js.log("key found : " ++ k));
  };

  let path = (scheme, ~index) =>
    Future.make(resolve => {
      let suffix = index->Js.Int.toString;
      if (scheme->Js.String2.includes("?")) {
        resolve(Ok(scheme->Js.String2.replace("?", suffix)));
      } else if (index == 0) {
        resolve(Ok(scheme));
      } else {
        resolve(
          Ok(scheme->Js.String2.replace("/0'", "/" ++ suffix ++ "'")),
        );
      };
      resolve(Error("Invalid index!"));
    });

  let derive = (~settings, ~index, ~name, ~password) =>
    Future.mapOk2(
      secretAt(~settings, index),
      recoveryPhraseAt(~settings, index, ~password),
      (secret, recoveryPhrase) => {
      path(secret.derivationScheme, ~index=secret.addresses->Array.length)
      ->Future.flatMapOk(path =>
          path->HD.edesk(recoveryPhrase->HD.seed, ~password)
        )
      ->Future.flatMapOk(edesk => import(~settings, edesk, name, ~password))
      ->Future.tapOk(address =>
          {
            ...secret,
            addresses: Array.concat(secret.addresses, [|address|]),
          }
          ->updateSecretAt(~settings, index)
        )
    })
    ->Future.flatMapOk(update => update);

  let unsafeDelete = (~settings, name) =>
    settings
    ->AppSettings.sdk
    ->TezosSDK.forgetAddress(name)
    ->Future.mapError(handleSdkError);

  let delete = (~settings, name) =>
    Aliases.getAddressForAlias(~settings, name)
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
      Aliases.getAliasMap(~settings),
      (secrets, aliases) => {
      secrets[index]
      ->Option.map(secret =>
          secret.addresses->Array.keepMap(aliases->Map.String.get)
        )
      ->FutureEx.fromOption(
          ~error="Secret at index " ++ index->Int.toString ++ " not found!",
        )
      ->Future.flatMapOk(array =>
          array->Array.reduce(Future.value(Ok([||])), (a, b) =>
            a->Future.flatMapOk(a =>
              b
              ->unsafeDelete(~settings)
              ->Future.mapOk(b => a->Array.concat([|b|]))
            )
          )
        )
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
    module ExplorerAPI = Explorer(Getter);
    network
    ->ExplorerAPI.get(address, ~limit=1, ())
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
    let name = baseName ++ " /" ++ index->Js.Int.toString;
    path(derivationScheme, ~index)
    ->Future.flatMapOk(path => path->HD.edesk(seed, ~password))
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
                  if (isValidated) {
                    scanSeed(
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
                    unsafeDelete(~settings, name)->Future.map(_ => Ok([||]));
                  }
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
      )
    ->Future.mapError(handleSdkError);

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

  let indexOfRecoveryPhrase = (~settings, recoveryPhrase, ~password) =>
    recoveryPhrases(~settings)
    ->Option.getWithDefault([||])
    ->Array.map(SecureStorage.Cipher.decrypt2(password))
    ->List.fromArray
    ->Future.all
    ->Future.map(List.toArray)
    ->Future.map(decryptedRecoveryPhrases =>
        decryptedRecoveryPhrases->Array.getBy(decryptedRecoveryPhrase =>
          decryptedRecoveryPhrase == Ok(recoveryPhrase)
        )
      );

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
    ->Future.mapError(_ => I18n.form_input_error#wrong_password)
    ->Future.flatMapOk(_ =>
        indexOfRecoveryPhrase(~settings, backupPhrase, ~password)
        ->Future.map(index =>
            switch (index) {
            | Some(_) => Error("Secret already imported!")
            | None => Ok(index)
            }
          )
      )
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
    Aliases.getAliasMap(~settings)
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

module Delegate = (Getter: GetterAPI) => {
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
    switch (settings->AppSettings.network) {
    | `Mainnet =>
      "https://api.baking-bad.org/v2/bakers"
      ->Getter.get
      ->Future.mapOk(Json.Decode.(array(Delegate.decode)))
    | `Testnet(_) =>
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

  let getDelegationInfoForAccount =
      (network, account: string)
      : Future.t(Belt.Result.t(option(delegationInfo), Js.String.t)) => {
    module ExplorerAPI = Explorer(Getter);
    module BalanceAPI = Balance;
    network
    ->ExplorerAPI.get(account, ~types=[|"delegation"|], ~limit=1, ())
    ->Future.flatMapOk(operations =>
        if (operations->Array.length == 0) {
          Future.value(Ok(None));
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
                    Ok(
                      {
                        initialBalance: ProtocolXTZ.zero,
                        delegate: "",
                        timestamp: Js.Date.make(),
                        lastReward: None,
                      }
                      ->Some,
                    ),
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
                      ->ExplorerAPI.get(
                          info.delegate,
                          ~types=[|"transaction"|],
                          ~destination=account,
                          ~limit=1,
                          (),
                        )
                      ->Future.mapOk(operations =>
                          if (operations->Array.length == 0) {
                            info->Some;
                          } else {
                            switch (firstOperation.payload) {
                            | Business(payload) =>
                              switch (payload.payload) {
                              | Transaction(payload) =>
                                {...info, lastReward: Some(payload.amount)}
                                ->Some
                              | _ => info->Some
                              }
                            };
                          }
                        )
                    );
                }
              | None =>
                Js.log("No delegation set");
                Future.value(Ok(None));
              }
            | _ => Future.value(Error("Invalid operation type!"))
            }
          };
        }
      );
  };
};

module Tokens = (Getter: GetterAPI) => {
  type error =
    | OperationNotRunnableOffchain(string)
    | SimulationNotAvailable(string)
    | InjectionNotImplemented(string)
    | OffchainCallNotImplemented(string)
    | BackendError(string);

  let printError = (fmt, err) => {
    switch (err) {
    | OperationNotRunnableOffchain(s) =>
      Format.fprintf(fmt, "Operation '%s' cannot be run offchain.", s)
    | SimulationNotAvailable(s) =>
      Format.fprintf(fmt, "Operation '%s' is not simulable.", s)
    | InjectionNotImplemented(s) =>
      Format.fprintf(fmt, "Operation '%s' injection is not implemented", s)
    | OffchainCallNotImplemented(s) =>
      Format.fprintf(
        fmt,
        "Operation '%s' offchain call is not implemented",
        s,
      )
    | BackendError(s) => Format.fprintf(fmt, "%s", s)
    };
  };

  let handleTaquitoError = e => e->handleTaquitoError->BackendError;

  let errorToString = err => Format.asprintf("%a", printError, err);

  let getTokenViewer = settings => URL.tokenViewer(settings)->Getter.get;

  let checkTokenContract = (settings, addr) => {
    URL.checkToken(settings, addr)
    ->Getter.get
    ->Future.map(result => {
        switch (result) {
        | Ok(json) =>
          switch (Js.Json.classify(json)) {
          | Js.Json.JSONTrue => Ok(true)
          | JSONFalse => Ok(false)
          | _ => Error("Error")
          }
        | Error(e) => Error(e)
        }
      });
  };

  let injectBatch = (settings, transfers, ~source, ~password) => {
    let transfers = source =>
      transfers
      ->List.map(
          ({token, amount, destination, tx_options}: Token.Transfer.elt) =>
          ReTaquito.FA12Operations.toRawTransfer(
            ~token,
            ~amount=amount->Token.Repr.toBigNumber,
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
    let endpoint = settings->AppSettings.endpoint;
    ReTaquito.FA12Operations.Estimate.transfer(
      ~endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~tokenContract=transfer.Token.Transfer.token,
      ~source,
      ~dest=transfer.Token.Transfer.destination,
      ~amount=transfer.Token.Transfer.amount->Token.Repr.toBigNumber,
      ~fee=?
        transfer.Token.Transfer.tx_options.fee
        ->Option.map(ProtocolXTZ.toInt64),
      ~gasLimit=?transfer.Token.Transfer.tx_options.gasLimit,
      ~storageLimit=?transfer.Token.Transfer.tx_options.storageLimit,
      (),
    )
    ->Future.mapOk(({totalCost, gasLimit, storageLimit, revealFee}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->ProtocolXTZ.fromMutezInt,
        }
      );
  };

  let batchEstimate = (settings, transfers, ~source, ~index=?, ()) => {
    let endpoint = settings->AppSettings.endpoint;

    let customValues =
      List.map(transfers, tx =>
        tx.Token.Transfer.tx_options->Simulation.extractCustomValues
      )
      ->List.toArray;

    let transfers = source =>
      transfers
      ->List.map(
          ({token, amount, destination, tx_options}: Token.Transfer.elt) =>
          ReTaquito.FA12Operations.toRawTransfer(
            ~token,
            ~amount=amount->Token.Repr.toBigNumber,
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
      ->ReTaquito.FA12Operations.prepareTransfers(source, endpoint);

    ReTaquito.FA12Operations.Estimate.batch(
      ~endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      (),
    )
    ->Future.flatMapOk(r =>
        ReTaquito.Estimate.handleEstimationResults(r, customValues, index)
      )
    ->Future.mapOk(({totalCost, gasLimit, storageLimit, revealFee}) =>
        Protocol.{
          fee: totalCost->ProtocolXTZ.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->ProtocolXTZ.fromMutezInt,
        }
      );
  };

  let simulate = (network, ~index=?, operation: Token.operation) =>
    switch (operation) {
    | Transfer({source, transfers: [elt], _}) =>
      transferEstimate(network, elt, source)
      ->Future.mapError(handleTaquitoError)
    | Transfer({source, transfers, _}) =>
      batchEstimate(network, transfers, ~source, ~index?, ())
      ->Future.mapError(handleTaquitoError)
    | _ =>
      Future.value(
        Error(SimulationNotAvailable(Token.operationEntrypoint(operation))),
      )
    };

  let transfer = (settings, transfer, source, password) => {
    ReTaquito.FA12Operations.transfer(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~tokenContract=transfer.Token.Transfer.token,
      ~source,
      ~dest=transfer.Token.Transfer.destination,
      ~amount=transfer.Token.Transfer.amount->Token.Repr.toBigNumber,
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
      ->Future.mapError(handleTaquitoError)
    | Transfer({source, transfers, _}) =>
      injectBatch(network, transfers, ~source, ~password)
      ->Future.mapError(handleTaquitoError)
    | _ =>
      Future.value(
        Error(
          InjectionNotImplemented(Token.operationEntrypoint(operation)),
        ),
      )
    };

  let callGetOperationOffline = (settings, operation: Token.operation) =>
    if (offline(operation)) {
      switch (operation) {
      | GetBalance({token, address, _}) =>
        URL.getTokenBalance(settings, token, address)
        ->Getter.get
        ->Future.flatMapOk(res => {
            switch (res->Js.Json.decodeString) {
            | None => Token.Repr.zero->Ok->Future.value
            | Some(v) =>
              v
              ->Token.Repr.fromNatString
              ->FutureEx.fromOption(~error="cannot read Token amount: " ++ v)
            }
          })
        ->Future.mapError(s => BackendError(s))
      | _ =>
        Future.value(
          Error(
            OffchainCallNotImplemented(Token.operationEntrypoint(operation)),
          ),
        )
      };
    } else {
      Future.value(
        Error(
          OperationNotRunnableOffchain(Token.operationEntrypoint(operation)),
        ),
      );
    };
};
