include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type injection = {
  operation: Injection.operation,
  password: string,
};

let useCreate = (~sideEffect=?, ~network) => {
  let set = (~config, {operation, password}) =>
    (network, config)->OperationsAPI.inject(operation, ~password);

  ApiRequest.useStoreSetter(
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
    (network, config)->OperationsAPI.simulate(operation);
  ApiRequest.useStoreSetter(~set, ~kind=Logs.Operation, ());
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
    ApiRequest.useStoreGetter(~get, ~kind=Logs.Operation, ~setRequest, ());

  React.useEffect3(
    () => {
      address->Common.Lib.Option.iter(address =>
        if (address != "" && request == NotAsked) {
          getRequest((network, address));
        }
      );

      None;
    },
    (network, request, address),
  );

  request;
};
