/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

include ApiRequest

/* Create */

type injection = {
  operation: Protocol.batch,
  signingIntent: TaquitoAPI.Signer.intent,
}

type operationsResponse = {
  operations: array<Operation.t>,
  currentLevel: int,
}

let keepNonFormErrors = x =>
  switch x {
  | ReTaquitoError.LedgerInitTimeout
  | ReTaquitoError.LedgerInit(_)
  | ReTaquitoError.LedgerKeyRetrieval
  | ReTaquitoError.LedgerDenied
  | ReTaquitoError.LedgerNotReady
  | SecureStorage.WrongPassword => false
  | _ => true
  }

let useCreate = (~sideEffect=?, ()) => {
  let set = (~config, {operation, signingIntent}) =>
    config->NodeAPI.Operation.run(operation.source, operation.managers, ~signingIntent)

  ApiRequest.useSetter(
    ~toast=true,
    ~set,
    ~kind=Logs.Operation,
    ~keepError=keepNonFormErrors,
    ~sideEffect?,
    (),
  )
}

/* Simulate */

let useSimulate = () => {
  let set = (~config, (source, managers)) => config->NodeAPI.Simulation.run(source, managers)

  ApiRequest.useSetter(~set, ~kind=Logs.Operation, ~keepError=keepNonFormErrors, ())
}

let waitForConfirmation = (config: ConfigContext.env, hash) =>
  config.network.endpoint->TaquitoAPI.Operations.confirmation(~hash, ())

/* Get list */

let useLoad = (~requestState, ~limit=?, ~types=?, ~address: PublicKeyHash.t, ()) => {
  let get = (~config: ConfigContext.env, address) => {
    let operations = ServerAPI.Explorer.Tzkt.getOperations(config.network, address, ~limit?, ~types?, ())
    let currentLevel = ServerAPI.Explorer.Tzkt.getBlocksCount(config.network)
    let f = (operations, currentLevel) => {
      switch (operations, currentLevel) {
      | (Ok(operations), Ok(currentLevel)) =>
        Ok({operations: operations, currentLevel: currentLevel})
      | (Error(_) as e, _)
      | (_, Error(_) as e) => e
      }
    }
    Promise.map2(operations, currentLevel, f)
  }

  ApiRequest.useLoader(~get, ~kind=Logs.Operation, ~requestState, address)
}
