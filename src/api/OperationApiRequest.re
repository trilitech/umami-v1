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
    (network, config)->OperationsAPI.simulate(operation);
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
