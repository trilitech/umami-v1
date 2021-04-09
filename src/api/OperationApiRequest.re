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

let useCreate = (~sideEffect=?, ()) => {
  let set = (~settings, {operation, password}) => {
    switch (operation) {
    | Protocol(operation) =>
      settings
      ->API.Operation.run(operation, ~password)
      ->Future.mapError(e =>
          switch (e) {
          | ReTaquito.WrongPassword => I18n.form_input_error#wrong_password
          | ReTaquito.Generic(e) => e
          }
        )

    | Token(operation) =>
      settings
      ->TokensApiRequest.TokensAPI.inject(operation, ~password)
      ->Future.mapError(TokensApiRequest.TokensAPI.errorToString)
    };
  };

  ApiRequest.useSetter(
    ~toast=true,
    ~set,
    ~kind=Logs.Operation,
    ~sideEffect?,
    (),
  );
};

/* Simulate */

let useSimulate = () => {
  let set = (~settings, operation) =>
    switch (operation) {
    | Operation.Simulation.Protocol(operation, index) =>
      settings->API.Simulation.run(~index?, operation)
    | Operation.Simulation.Token(operation) =>
      settings
      ->TokensApiRequest.TokensAPI.simulate(operation)
      ->Future.mapError(TokensApiRequest.TokensAPI.errorToString)
    };

  ApiRequest.useSetter(~set, ~kind=Logs.Operation, ());
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
    ApiRequest.useGetter(~get, ~kind=Logs.Operation, ~setRequest, ());

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
