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
