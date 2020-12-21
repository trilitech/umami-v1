include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type operation =
  | Regular(Injection.operation)
  | Token(Token.operation);

type injection = {
  operation,
  password: string,
};

let regular = (operation, password) => {
  operation: Regular(operation),
  password,
};

let token = (operation, password) => {
  operation: Token(operation),
  password,
};

let useCreate = (~sideEffect=?, ~network) => {
  let set = (~config, {operation, password}) => {
    switch (operation) {
    | Regular(operation) =>
      (network, config)->OperationsAPI.inject(operation, ~password)
    | Token(operation) =>
      (network, config)
      ->TokensApiRequest.TokensAPI.inject(operation, ~password)
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

let useSimulate = (~network) => {
  let set = (~config, operation) =>
    switch (operation) {
    | Regular(operation) =>
      (network, config)->OperationsAPI.simulate(operation)
    | Token(operation) =>
      (network, config)->TokensApiRequest.TokensAPI.simulate(operation)
    };

  ApiRequest.useSetter(~set, ~kind=Logs.Operation, ());
};

/* Get list */

let useLoad =
    (
      ~network,
      ~requestState as (request, setRequest),
      ~limit=?,
      ~types=?,
      ~address: option(string),
      (),
    ) => {
  let get = (~config, (network, address)) => {
    (network, config)
    ->OperationsAPI.get(address, ~limit?, ~types?, ~mempool=true, ());
  };

  let getRequest =
    ApiRequest.useGetter(~get, ~kind=Logs.Operation, ~setRequest, ());

  let isMounted = ReactUtils.useIsMonted();
  React.useEffect4(
    () => {
      address->Common.Lib.Option.iter(address => {
        let shouldReload = ApiRequest.conditionToLoad(request, isMounted);
        if (address != "" && shouldReload) {
          getRequest((network, address));
        };
      });

      None;
    },
    (isMounted, network, request, address),
  );

  request;
};
