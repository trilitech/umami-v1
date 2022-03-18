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
    | UnreadableTokenAmount(s) => I18n.Errors.cannot_read_token(s)->Some
    | InvalidOperationType => I18n.Errors.invalid_operation_type->Some
    | IllformedTokenContract => I18n.Errors.illformed_token_contract->Some
    | _ => None,
  );

module Accounts = {
  let exists = (config, account) => {
    let%AwaitMap json = URL.Explorer.accountExists(config, ~account)->URL.get;
    switch (Js.Json.classify(json)) {
    | Js.Json.JSONTrue => true
    | _ => false
    };
  };
};

module Balance = {
  let get = (config: ConfigContext.env, address, ~params=?, ()) => {
    config.network.endpoint
    ->TaquitoAPI.Rpc.getBalance(~address, ~params?, ());
  };
};

module Simulation = {
  let extractCustomValues =
    fun
    | Protocol.Delegation({options})
    | Origination({options})
    | Transfer({options}) => (
        options.fee->Option.map(fee => fee->Tez.unsafeToMutezInt),
        options.storageLimit,
        options.gasLimit,
      );

  let run = (config: ConfigContext.env, ops: Protocol.batch) => {
    let customValues = ops.managers->Array.map(op => op->extractCustomValues);

    TaquitoAPI.Batch.Estimate.run(
      ~endpoint=config.network.endpoint,
      ~baseDir=config.baseDir(),
      ~source=ops.source.Account.address,
      ~customValues,
      ~ops,
      (),
    );
  };
};

module MapString = Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module DelegateMaker = (Get: {let get: URL.t => Promise.t(Js.Json.t);}) => {
  let getForAccount = (config: ConfigContext.env, account) => {
    let%AwaitMap res =
      TaquitoAPI.Delegation.get(config.network.endpoint, account);

    res->Option.flatMap(delegate => account == delegate ? None : res);
  };

  let getBakers = (config: ConfigContext.env) =>
    switch (config.network.chain) {
    | chain when chain == `Mainnet =>
      URL.External.bakingBadBakers
      ->URL.get
      ->Promise.mapOk(Json.Decode.(array(Delegate.decode)))
    | _ => [||]->Promise.ok
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
      (network, delegate, account, firstOperation: Operation.t) => {
    let%Await balance =
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

    let%AwaitMap operations =
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
      switch ((firstOperation.payload: Operation.payload)) {
      | Transaction(Token(payload, _, _))
      | Transaction(Tez(payload)) =>
        {...info, lastReward: Some(payload.amount)}->Some
      | _ => info->Some
      };
    };
  };

  let getDelegationInfoForAccount =
      (network, account: PublicKeyHash.t): Promise.t(option(delegationInfo)) => {
    let%Await operations =
      network->ExplorerAPI.getOperations(
        account,
        ~types=[|"delegation"|],
        ~limit=1,
        (),
      );

    if (operations->Array.length == 0) {
      Promise.ok(None);
    } else {
      let firstOperation = operations->Array.getUnsafe(0);

      let%Await payload =
        switch (firstOperation.payload) {
        | Delegation(payload) => payload->Promise.ok
        | _ => InvalidOperationType->Promise.err
        };

      switch (payload.delegate) {
      | None => Promise.none()
      | Some(delegate) when account == delegate =>
        {
          initialBalance: Tez.zero,
          delegate: None,
          timestamp: Js.Date.make(),
          lastReward: None,
        }
        ->Promise.some
      | Some(delegate) =>
        extractInfoFromDelegate(network, delegate, account, firstOperation)
      };
    };
  };
};

module OperationRepr = Operation;

module Operation = {
  let run = (config: ConfigContext.env, ops: Protocol.batch, ~signingIntent) =>
    TaquitoAPI.Batch.run(
      ~endpoint=config.network.endpoint,
      ~baseDir=config.baseDir(),
      ~source=ops.source.address,
      ~ops,
      ~signingIntent,
      (),
    );
};

module Delegate = DelegateMaker(URL);

module Tokens = {
  type tokenKind = [ OperationRepr.Transaction.tokenKind | `NotAToken];

  let checkTokenContract = (config, contract: PublicKeyHash.t) => {
    let%AwaitRes json = URL.Explorer.checkToken(config, ~contract)->URL.get;
    switch (Js.Json.classify(json)) {
    | Js.Json.JSONString("fa1-2") => Ok(`KFA1_2)
    | Js.Json.JSONString("fa2") => Ok(`KFA2)
    | Js.Json.JSONNull => Ok(`NotAToken)
    | _ => Error(IllformedTokenContract)
    };
  };

  let runFA12GetBalance = (config, ~address, ~token) => {
    let%Await json =
      config
      ->URL.Endpoint.runView
      ->URL.postJson(
          URL.Endpoint.fa12GetBalanceInput(
            ~config,
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
    | None => Token.Unit.zero->Promise.ok
    | Some(v) =>
      v
      ->Token.Unit.fromNatString
      ->Result.mapError(_ => UnreadableTokenAmount(v))
      ->Promise.value
    };
  };

  let callFA2BalanceOf = (config, address, token, tokenId) => {
    let input =
      URL.Endpoint.fa2BalanceOfInput(
        ~config,
        ~contract=token,
        ~account=address,
        ~tokenId,
      );

    let%Await json = config->URL.Endpoint.runView->URL.postJson(input);

    let res = JsonEx.(decode(json, MichelsonDecode.fa2BalanceOfDecoder));

    switch (res) {
    | Ok([|((_pkh, _tokenId), v)|]) =>
      v
      ->Token.Unit.fromNatString
      ->Result.mapError(_ => UnreadableTokenAmount(v))
      ->Promise.value
    | Error(_)
    | Ok(_) => Token.Unit.zero->Promise.ok
    };
  };
};

module Signature = {
  let signPayload =
      (config: ConfigContext.env, ~source, ~signingIntent, ~payload) => {
    TaquitoAPI.Signature.signPayload(
      ~baseDir=config.baseDir(),
      ~source,
      ~signingIntent,
      ~payload,
    );
  };
};
