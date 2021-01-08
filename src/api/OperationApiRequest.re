include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

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
      settings->OperationsAPI.inject(operation, ~password)
    | Token(operation) =>
      settings->TokensApiRequest.TokensAPI.inject(operation, ~password)
    };
  };

  ApiRequest.useSetter(
    ~toast=false,
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
      settings->OperationsAPI.simulate(~index?, operation)
    | Operation.Simulation.Token(operation) =>
      settings->TokensApiRequest.TokensAPI.simulate(operation)
    };

  ApiRequest.useSetter(~set, ~kind=Logs.Operation, ());
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
    settings->OperationsAPI.get(address, ~limit?, ~types?, ~mempool=true, ());
  };

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Operation, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect3(
    () => {
      address->Common.Lib.Option.iter(address => {
        let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
        if (address != "" && shouldReload) {
          getRequest(address);
        };
      });

      None;
    },
    (isMounted, request, address),
  );

  request;
};
