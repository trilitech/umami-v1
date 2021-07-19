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

include ApiRequest;

/* Create */

type injection = {
  operation: Operation.t,
  password: string,
};

type operationsResponse = {
  operations: array(Operation.Read.t),
  currentLevel: int,
};

let transfer = (operation, password) => {
  operation: Operation.transaction(operation),
  password,
};

let delegate = (d, password) => {
  operation: Operation.delegation(d),

  password,
};

let token = (operation, password) => {
  operation: Token(operation),
  password,
};

let errorToString = ErrorHandler.toString;

let filterOutFormError =
  fun
  | ErrorHandler.Taquito(WrongPassword) => false
  | _ => true;

let useCreate = (~sideEffect=?, ()) => {
  let set = (~settings, {operation, password}) => {
    switch (operation) {
    | Protocol(operation) =>
      settings
      ->NodeAPI.Operation.run(operation, ~password)
      ->Future.mapError(ErrorHandler.taquito)

    | Token(operation) =>
      settings
      ->NodeAPI.Tokens.inject(operation, ~password)
      ->Future.mapError(e => e)

    | Transfer(t) =>
      settings
      ->NodeAPI.Operation.batch(t.transfers, ~source=t.source, ~password)
      ->Future.mapError(ErrorHandler.taquito)
    };
  };

  ApiRequest.useSetter(
    ~toast=true,
    ~set,
    ~kind=Logs.Operation,
    ~keepError=filterOutFormError,
    ~errorToString,
    ~sideEffect?,
    (),
  );
};

/* Simulate */

let useSimulate = () => {
  let set = (~settings, operation) =>
    switch (operation) {
    | Operation.Simulation.Protocol(operation, index) =>
      settings
      ->NodeAPI.Simulation.run(~index?, operation)
      ->Future.mapError(ErrorHandler.taquito)
    | Operation.Simulation.Token(operation, index) =>
      settings->NodeAPI.Tokens.simulate(~index?, operation)
    | Operation.Simulation.Transfer(t, index) =>
      settings
      ->NodeAPI.Simulation.batch(t.transfers, ~source=t.source, ~index?, ())
      ->Future.mapError(ErrorHandler.taquito)
    };

  ApiRequest.useSetter(
    ~set,
    ~kind=Logs.Operation,
    ~errorToString,
    ~keepError=filterOutFormError,
    (),
  );
};

let waitForConfirmation = (settings, hash) => {
  settings->AppSettings.endpoint->ReTaquito.Operations.confirmation(hash, ());
};

/* Get list */

let useLoad =
    (~requestState, ~limit=?, ~types=?, ~address: PublicKeyHash.t, ()) => {
  let get = (~settings, address) => {
    let operations =
      settings->ServerAPI.Explorer.getOperations(
        address,
        ~limit?,
        ~types?,
        ~mempool=true,
        (),
      );
    let currentLevel =
      Network.monitor(AppSettings.explorer(settings))
      ->Future.mapOk(monitor => monitor.nodeLastBlock)
      ->Future.mapError(Network.errorMsg);

    let f = (operations, currentLevel) =>
      switch (operations, currentLevel) {
      | (Ok(operations), Ok(currentLevel)) =>
        Ok({operations, currentLevel})
      | (Error(_) as e, _)
      | (_, Error(_) as e) => e
      };

    Future.map2(operations, currentLevel, f);
  };

  ApiRequest.useLoader(~get, ~kind=Logs.Operation, ~requestState, address);
};
