/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

open ServerAPI;

open Let;

type Errors.t +=
  | IllformedTokenContract
  | InvalidOperationType
  | UnreadableTokenAmount(string);

let () =
  Errors.registerHandler(
    "Node",
    fun
    | UnreadableTokenAmount(s) => I18n.errors#cannot_read_token(s)->Some
    | InvalidOperationType => I18n.errors#invalid_operation_type->Some
    | IllformedTokenContract => I18n.errors#illformed_token_contract->Some
    | _ => None,
  );

module Balance = {
  let get = (config, address, ~params=?, ()) => {
    ConfigUtils.endpoint(config)
    ->TaquitoAPI.Balance.get(~address, ~params?, ());
  };
};

module Simulation = {
  let extractCustomValues = (tx_options: ProtocolOptions.transferOptions) => (
    tx_options.fee->Option.map(fee => fee->Tez.unsafeToMutezInt),
    tx_options.storageLimit,
    tx_options.gasLimit,
  );

  let batch = (config, transfers, ~source, ~index=?, ()) => {
    let customValues =
      List.map(transfers, tx => tx.Transfer.tx_options->extractCustomValues)
      ->List.toArray;

    let%FRes r =
      TaquitoAPI.Transfer.Estimate.batch(
        ~endpoint=config->ConfigUtils.endpoint,
        ~baseDir=config->ConfigUtils.baseDir,
        ~source,
        ~transfers=transfers->TaquitoAPI.Transfer.prepareTransfers,
        (),
      );
    open ReTaquito.Toolkit.Estimation;

    let%FResMap {customFeeMutez, gasLimit, storageLimit, revealFee} =
      TaquitoAPI.handleEstimationResults(r, customValues, index)
      ->Future.value;

    Protocol.{
      fee: customFeeMutez->Tez.fromMutezInt,
      gasLimit,
      storageLimit,
      revealFee: revealFee->Tez.fromMutezInt,
    };
  };

  let setDelegate = (config, delegation: Protocol.delegation) => {
    let%FResMap {
      customFeeMutez,
      burnFeeMutez,
      gasLimit,
      storageLimit,
      revealFee,
    } =
      TaquitoAPI.Delegate.Estimate.set(
        ~endpoint=config->ConfigUtils.endpoint,
        ~baseDir=config->ConfigUtils.baseDir,
        ~source=delegation.Protocol.source,
        ~delegate=?delegation.Protocol.delegate,
        ~fee=?delegation.Protocol.options.fee,
        (),
      );
    Protocol.{
      fee: (customFeeMutez + burnFeeMutez)->Tez.fromMutezInt,
      gasLimit,
      storageLimit,
      revealFee: revealFee->Tez.fromMutezInt,
    };
  };

  let run = (config, ~index=?, operation: Protocol.t) => {
    switch (operation, index) {
    | (Delegation(d), _) => setDelegate(config, d)
    | (Transaction({transfers, source}), None) =>
      batch(config, transfers, ~source, ())
    | (Transaction({transfers, source}), Some(index)) =>
      batch(config, transfers, ~source, ~index, ())
    };
  };
};

module MapString = Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module DelegateMaker =
       (Get: {let get: URL.t => Future.t(Result.t(Js.Json.t, Errors.t));}) => {
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

  let getForAccount = (config, account) => {
    let%FResMap res =
      TaquitoAPI.Delegate.get(config->ConfigUtils.endpoint, account);

    res->Option.flatMap(delegate => account == delegate ? None : res);
  };

  let getBakers = (config: ConfigFile.t) =>
    switch (config->ConfigUtils.chainId) {
    | chain when chain == Network.mainnetChain =>
      URL.External.bakingBadBakers
      ->URL.get
      ->Future.mapOk(Json.Decode.(array(Delegate.decode)))
    | _ => [||]->FutureEx.ok
    };

  type delegationInfo = {
    initialBalance: Tez.t,
    delegate: option(PublicKeyHash.t),
    timestamp: Js.Date.t,
    lastReward: option(Tez.t),
  };

  module ExplorerAPI = ServerAPI.ExplorerMaker(Get);
  module BalanceAPI = Balance;

  let extractInfoFromDelegate =
      (network, delegate, account, firstOperation: Operation.Read.t) => {
    let%FRes balance =
      network->BalanceAPI.get(
        account,
        ~params={block: firstOperation.level->string_of_int},
        (),
      );
    let info = {
      initialBalance: balance,
      delegate: Some(delegate),
      timestamp: firstOperation.timestamp,
      lastReward: None,
    };

    let%FResMap operations =
      network->ExplorerAPI.getOperations(
        delegate,
        ~types=[|"transaction"|],
        ~destination=account,
        ~limit=1,
        (),
      );

    if (operations->Array.length == 0) {
      info->Some;
    } else {
      switch ((firstOperation.payload: Operation.Read.payload)) {
      | Transaction(Token(payload, _))
      | Transaction(Tez(payload)) =>
        {...info, lastReward: Some(payload.amount)}->Some
      | _ => info->Some
      };
    };
  };

  let getDelegationInfoForAccount =
      (network, account: PublicKeyHash.t)
      : Future.t(Belt.Result.t(option(delegationInfo), Errors.t)) => {
    let%FRes operations =
      network->ExplorerAPI.getOperations(
        account,
        ~types=[|"delegation"|],
        ~limit=1,
        (),
      );

    if (operations->Array.length == 0) {
      Future.value(Ok(None));
    } else {
      let firstOperation = operations->Array.getUnsafe(0);

      let%FRes payload =
        switch (firstOperation.payload) {
        | Delegation(payload) => payload->FutureEx.ok
        | _ => InvalidOperationType->FutureEx.err
        };

      switch (payload.delegate) {
      | None => FutureEx.none()
      | Some(delegate) when account == delegate =>
        {
          initialBalance: Tez.zero,
          delegate: None,
          timestamp: Js.Date.make(),
          lastReward: None,
        }
        ->FutureEx.some
      | Some(delegate) =>
        extractInfoFromDelegate(network, delegate, account, firstOperation)
      };
    };
  };
};

module Operation = {
  let batch = (config, transfers, ~source, ~signingIntent) => {
    let%FResMap op =
      TaquitoAPI.Transfer.batch(
        ~endpoint=config->ConfigUtils.endpoint,
        ~baseDir=config->ConfigUtils.baseDir,
        ~source,
        ~transfers=transfers->TaquitoAPI.Transfer.prepareTransfers,
        ~signingIntent,
        (),
      );
    op.hash;
  };

  let setDelegate =
      (config, Protocol.{delegate, source, options}, ~signingIntent) => {
    let%FResMap op =
      TaquitoAPI.Delegate.set(
        ~endpoint=config->ConfigUtils.endpoint,
        ~baseDir=config->ConfigUtils.baseDir,
        ~source,
        ~delegate,
        ~signingIntent,
        ~fee=?options.fee,
        (),
      );
    op.hash;
  };

  let run = (config, operation: Protocol.t, ~signingIntent) =>
    switch (operation) {
    | Delegation(d) => setDelegate(config, d, ~signingIntent)

    | Transaction({transfers, source}) =>
      batch(config, transfers, ~source, ~signingIntent)
    };
};

module Delegate = DelegateMaker(URL);

module Tokens = {
  let checkTokenContract = (config, contract: PublicKeyHash.t) => {
    let%FlatRes json = URL.Explorer.checkToken(config, ~contract)->URL.get;
    switch (Js.Json.classify(json)) {
    | Js.Json.JSONTrue => Ok(true)
    | JSONFalse => Ok(false)
    | _ => Error(IllformedTokenContract)
    };
  };

  let runFA12GetBalance = (config, ~address, ~token) => {
    let%FRes json =
      config
      ->URL.Endpoint.runView
      ->URL.postJson(
          URL.Endpoint.fa12GetBalanceInput(
            ~settings=config,
            ~contract=token,
            ~account=address,
          ),
        );

    /* bs-json raises exceptions instead of returning options */
    let res =
      try(Json.Decode.(json |> field("data", field("int", string)))->Some) {
      | Json.Decode.DecodeError(_) => None
      };

    switch (res) {
    | None => Token.Unit.zero->FutureEx.ok
    | Some(v) =>
      v
      ->Token.Unit.fromNatString
      ->FutureEx.fromOption(~error=UnreadableTokenAmount(v))
    };
  };

  let callFA2BalanceOf = (config, address, token, tokenId) => {
    let input =
      URL.Endpoint.fa2BalanceOfInput(
        ~settings=config,
        ~contract=token,
        ~account=address,
        ~tokenId,
      );

    let%FRes json = config->URL.Endpoint.runView->URL.postJson(input);

    let res = JsonEx.(decode(json, MichelsonDecode.fa2BalanceOfDecoder));

    switch (res) {
    | Ok([|((_pkh, _tokenId), v)|]) =>
      v
      ->Token.Unit.fromNatString
      ->FutureEx.fromOption(~error=UnreadableTokenAmount(v))
    | Error(_)
    | Ok(_) => Token.Unit.zero->FutureEx.ok
    };
  };
};

module Signature = {
  let signPayload = (config, ~source, ~signingIntent, ~payload) => {
    TaquitoAPI.Signature.signPayload(
      ~baseDir=config->ConfigUtils.baseDir,
      ~source,
      ~signingIntent,
      ~payload,
    );
  };
};
