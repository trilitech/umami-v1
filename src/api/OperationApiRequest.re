include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type injection = {
  operation: Injection.operation,
  password: string,
};

let useCreateOperation = network => {
  let set = (~config, {operation, password}) =>
    (network, config)->OperationsAPI.inject(operation, ~password);
  ApiRequest.useSetter(set, Error.Operation, ());
};

/* Simulate */

let useSimulateOperation = network => {
  let set = (~config, operation) =>
    (network, config)->OperationsAPI.simulate(operation);
  ApiRequest.useSetter(set, Error.Operation, ());
};

/* Get list */

let useGetOperations = (~limit=?, ~types=?, ()) => {
  let setOperations = StoreContext.useSetOperations();

  let get = (~config, (network, account: Account.t)) => {
    (network, config)
    ->OperationsAPI.get(account.address, ~limit?, ~types?, ~mempool=true, ())
    ->Future.tapOk(res => setOperations(_ => res));
  };

  ApiRequest.useGetter(get, Error.Operation);
};
