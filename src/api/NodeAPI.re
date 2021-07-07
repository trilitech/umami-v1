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
open Delegate;

module Balance = {
  let get = (settings, address, ~params=?, ()) => {
    AppSettings.endpoint(settings)
    ->ReTaquito.Balance.get(~address, ~params?, ())
    ->Future.mapOk(Tez.ofInt64);
  };
};

module Simulation = {
  let extractCustomValues = (tx_options: ProtocolOptions.transferOptions) => (
    tx_options.fee->Option.map(fee => fee->Tez.unsafeToMutezInt),
    tx_options.storageLimit,
    tx_options.gasLimit,
  );

  let batch = (settings, transfers, ~source, ~index=?, ()) => {
    let customValues =
      List.map(transfers, tx => tx.Transfer.tx_options->extractCustomValues)
      ->List.toArray;

    let transfers = (cache, source) =>
      transfers->ReTaquito.Transfer.prepareTransfers(cache, source);

    ReTaquito.Transfer.Estimate.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      (),
    )
    ->Future.flatMapOk(r =>
        ReTaquito.Estimate.handleEstimationResults(r, customValues, index)
      )
    ->Future.mapOk(
        ({customFeeMutez, burnFeeMutez, gasLimit, storageLimit, revealFee}) => {
        Protocol.{
          fee: (customFeeMutez + burnFeeMutez)->Tez.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->Tez.fromMutezInt,
        }
      });
  };

  let setDelegate = (settings, delegation: Protocol.delegation) => {
    ReTaquito.Estimate.setDelegate(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source=delegation.Protocol.source,
      ~delegate=?delegation.Protocol.delegate,
      ~fee=?delegation.Protocol.options.fee,
      (),
    )
    ->Future.mapOk(
        ({customFeeMutez, burnFeeMutez, gasLimit, storageLimit, revealFee}) =>
        Protocol.{
          fee: (customFeeMutez + burnFeeMutez)->Tez.fromMutezInt,
          gasLimit,
          storageLimit,
          revealFee: revealFee->Tez.fromMutezInt,
        }
      );
  };

  let run = (settings, ~index=?, operation: Protocol.t) => {
    switch (operation, index) {
    | (Delegation(d), _) => setDelegate(settings, d)
    | (Transaction({transfers, source}), None) =>
      batch(settings, transfers, ~source, ())
    | (Transaction({transfers, source}), Some(index)) =>
      batch(settings, transfers, ~source, ~index, ())
    };
  };
};

module Operation = {
  let batch = (settings, transfers, ~source, ~password) => {
    let transfers = (cache, source) =>
      transfers->ReTaquito.Transfer.prepareTransfers(cache, source);

    ReTaquito.Transfer.batch(
      ~endpoint=settings->AppSettings.endpoint,
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~transfers,
      ~password,
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
      ~fee=?options.fee->Option.map(Tez.toInt64),
      (),
    )
    ->Future.mapOk((op: ReTaquito.Toolkit.operationResult) => op.hash);
  };

  let run = (settings, operation: Protocol.t, ~password) =>
    switch (operation) {
    | Delegation(d) => setDelegate(settings, d, ~password)

    | Transaction({transfers, source}) =>
      batch(settings, transfers, ~source, ~password)
    };
};

module MapString = Map.String;

module Mnemonic = {
  [@bs.module "bip39"] external generate: unit => string = "generateMnemonic";
};

module DelegateMaker =
       (Get: {let get: URL.t => Future.t(Result.t(Js.Json.t, string));}) => {
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
      URL.External.bakingBadBakers
      ->URL.get
      ->Future.mapOk(Json.Decode.(array(Delegate.decode)))
    | `Testnet(_) =>
      Future.value(
        Ok([|
          {name: "zebra", address: "tz1LbSsDSmekew3prdDGx1nS22ie6jjBN6B3"},
        |]),
      )
    };

  type delegationInfo = {
    initialBalance: Tez.t,
    delegate: string,
    timestamp: Js.Date.t,
    lastReward: option(Tez.t),
  };

  let getDelegationInfoForAccount =
      (network, account: string)
      : Future.t(Belt.Result.t(option(delegationInfo), Js.String.t)) => {
    module ExplorerAPI = ServerAPI.ExplorerMaker(Get);
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
                        initialBalance: Tez.zero,
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

module Delegate = DelegateMaker(URL);

module Tokens = {
  let checkTokenContract = (settings, contract) => {
    URL.Explorer.checkToken(settings, ~contract)
    ->URL.get
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

  let batchEstimate = (settings, transfers, ~source, ~index=?, ()) =>
    Simulation.batch(settings, transfers, ~source, ~index?, ());

  let batch = (settings, transfers, ~source, ~password) =>
    Operation.batch(settings, transfers, ~source, ~password);

  let offline = (operation: Token.operation) => {
    switch (operation) {
    | Transfer(_)
    | Approve(_) => false
    | GetBalance(_)
    | GetAllowance(_)
    | GetTotalSupply(_) => true
    };
  };

  let simulate = (network, ~index=?, operation: Token.operation) =>
    switch (operation) {
    | Transfer({source, transfers, _}) =>
      batchEstimate(network, transfers, ~source, ~index?, ())
      ->Future.mapError(e => e->ErrorHandler.Taquito)
    | _ =>
      Future.value(
        SimulationNotAvailable(Token.operationEntrypoint(operation))
        ->ErrorHandler.token
        ->Error,
      )
    };

  let inject = (network, operation: Token.operation, ~password) =>
    switch (operation) {
    | Transfer({source, transfers, _}) =>
      batch(network, transfers, ~source, ~password)
      ->Future.mapError(ErrorHandler.taquito)
    | _ =>
      Future.value(
        InjectionNotImplemented(Token.operationEntrypoint(operation))
        ->ErrorHandler.token
        ->Error,
      )
    };

  let callGetOperationOffline = (settings, operation: Token.operation) =>
    if (offline(operation)) {
      switch (operation) {
      | GetBalance({token, address, _}) =>
        URL.Explorer.getTokenBalance(
          settings,
          ~contract=token,
          ~account=address,
        )
        ->URL.get
        ->Future.flatMapOk(res => {
            switch (res->Js.Json.decodeString) {
            | None => Token.Unit.zero->Ok->Future.value
            | Some(v) =>
              v
              ->Token.Unit.fromNatString
              ->FutureEx.fromOption(~error="cannot read Token amount: " ++ v)
            }
          })
        ->Future.mapError(s => s->RawError->ErrorHandler.Token)
      | _ =>
        Future.value(
          OffchainCallNotImplemented(Token.operationEntrypoint(operation))
          ->ErrorHandler.token
          ->Error,
        )
      };
    } else {
      Future.value(
        OperationNotRunnableOffchain(Token.operationEntrypoint(operation))
        ->ErrorHandler.token
        ->Error,
      );
    };
};

module Signature = {
  let signPayload = (settings, ~source, ~password, ~payload) => {
    ReTaquito.Signature.signPayload(
      ~baseDir=settings->AppSettings.baseDir,
      ~source,
      ~password,
      ~payload,
    )
    ->Future.mapError(taquitoError => ErrorHandler.Taquito(taquitoError));
  };
};
