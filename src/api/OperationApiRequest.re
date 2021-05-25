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

open UmamiCommon;
include ApiRequest;
module Explorer = API.Explorer(API.TezosExplorer);

/* Create */

type injection = {
  operation: Operation.t,
  password: string,
};

let transfert = (operation, password) => {
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

let errorToString = API.Error.fromApiToString;

let filterOutFormError =
  fun
  | API.Error.Taquito(WrongPassword) => false
  | _ => true;

let useCreate = (~sideEffect=?, ()) => {
  let set = (~settings, {operation, password}) => {
    switch (operation) {
    | Protocol(operation) =>
      settings
      ->API.Operation.run(operation, ~password)
      ->Future.mapError(API.Error.taquito)

    | Token(operation) =>
      settings
      ->TokensApiRequest.API.inject(operation, ~password)
      ->Future.mapError(e => e)

    | Transfer(t) =>
      settings
      ->API.Operation.batch(t.transfers, ~source=t.source, ~password)
      ->Future.mapError(API.Error.taquito)
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
      ->API.Simulation.run(~index?, operation)
      ->Future.mapError(API.Error.taquito)
    | Operation.Simulation.Token(operation, index) =>
      settings->TokensApiRequest.API.simulate(~index?, operation)
    | Operation.Simulation.Transfer(t, index) =>
      settings
      ->API.Simulation.batch(t.transfers, ~source=t.source, ~index?, ())
      ->Future.mapError(API.Error.taquito)
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
    (
      ~requestState as (request, setRequest),
      ~limit=?,
      ~types=?,
      ~address: option(string),
      (),
    ) => {
  let get = (~settings, address) => {
    let operations =
      settings->Explorer.get(address, ~limit?, ~types?, ~mempool=true, ());
    let currentLevel =
      Network.monitor(AppSettings.explorer(settings))
      ->Future.mapOk(monitor => monitor.nodeLastBlock)
      ->Future.mapError(Network.errorMsg);

    let f = (operations, currentLevel) =>
      switch (operations, currentLevel) {
      | (Ok(operations), Ok(currentLevel)) =>
        Ok((operations, currentLevel))
      | (Error(_) as e, _)
      | (_, Error(_) as e) => e
      };

    Future.map2(operations, currentLevel, f);
  };

  let getRequest =
    ApiRequest.useGetter(
      ~get,
      ~kind=Logs.Operation,
      ~errorToString=x => x,
      ~setRequest,
      (),
    );

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      address->Lib.Option.iter(address => {
        let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
        if (address != "" && shouldReload) {
          getRequest(address)->ignore;
        };
      });

      None;
    },
    (isMounted, request, address),
  );

  request;
};
