include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type injection = {
  operation: Injection.operation,
  password: string,
};

let useCreate = () => {
  let network = StoreContext.useNetwork();
  let resetOperations = StoreContext.useResetOperations();

  let set = (~config, {operation, password}) =>
    (network, config)
    ->OperationsAPI.inject(operation, ~password)
    ->Future.tapOk(_ => resetOperations());

  ApiRequest.useSetter(~toast=false, set, Logs.Operation, ());
};

/* Simulate */

let useSimulate = network => {
  let set = (~config, operation) =>
    (network, config)->OperationsAPI.simulate(operation);
  ApiRequest.useSetter(set, Logs.Operation, ());
};

/* Get list */

let useGet = (~limit=?, ~types=?, address: option(string), ()) => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) =
    StoreContext.useOperationsRequestState(address);

  let get = (~config, (network, address)) => {
    (network, config)
    ->OperationsAPI.get(address, ~limit?, ~types?, ~mempool=true, ());
  };

  let getRequest = ApiRequest.useStoreGetter(get, Logs.Operation, setRequest);

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
