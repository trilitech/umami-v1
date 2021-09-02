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
  signingIntent: TaquitoAPI.Signer.intent,
};

type operationsResponse = {
  operations: array(Operation.Read.t),
  currentLevel: int,
};

let transfer = (operation, signingIntent) => {
  operation: Operation.transaction(operation),
  signingIntent,
};

let delegate = (d, signingIntent) => {
  operation: Operation.delegation(d),
  signingIntent,
};

let token = (operation, signingIntent) => {
  operation: Token(operation),
  signingIntent,
};

let filterOutFormError =
  fun
  | ReTaquitoError.LedgerInitTimeout
  | ReTaquitoError.LedgerInit(_)
  | ReTaquitoError.LedgerKeyRetrieval
  | ReTaquitoError.LedgerDenied
  | ReTaquitoError.LedgerNotReady
  | Errors.WrongPassword => false
  | _ => true;

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config, {operation, signingIntent}) => {
    switch (operation) {
    | Protocol(operation) =>
      config->NodeAPI.Operation.run(operation, ~signingIntent)

    | Token(operation) =>
      config->NodeAPI.Tokens.inject(operation, ~signingIntent)

    | Transfer(t) =>
      config->NodeAPI.Operation.batch(
        t.transfers,
        ~source=t.source,
        ~signingIntent,
      )
    };
  };

  ApiRequest.useSetter(
    ~toast=true,
    ~set,
    ~kind=Logs.Operation,
    ~keepError=filterOutFormError,
    ~sideEffect?,
    (),
  );
};

/* Simulate */

let useSimulate = () => {
  let set = (~config, operation) =>
    switch (operation) {
    | Operation.Simulation.Protocol(operation, index) =>
      config->NodeAPI.Simulation.run(~index?, operation)
    | Operation.Simulation.Token(operation, index) =>
      config->NodeAPI.Tokens.simulate(~index?, operation)
    | Operation.Simulation.Transfer(t, index) =>
      config->NodeAPI.Simulation.batch(
        t.transfers,
        ~source=t.source,
        ~index?,
        (),
      )
    };

  ApiRequest.useSetter(
    ~set,
    ~kind=Logs.Operation,
    ~keepError=filterOutFormError,
    (),
  );
};

let waitForConfirmation = (config, hash) => {
  config->ConfigUtils.endpoint->TaquitoAPI.Operations.confirmation(~hash, ());
};

/* Get list */

let useLoad =
    (~requestState, ~limit=?, ~types=?, ~address: PublicKeyHash.t, ()) => {
  let get = (~config, address) => {
    let operations =
      config->ServerAPI.Explorer.getOperations(address, ~limit?, ~types?, ());
    let currentLevel =
      Network.monitor(ConfigUtils.explorer(config))
      ->Future.mapOk(monitor => monitor.nodeLastBlock);

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
