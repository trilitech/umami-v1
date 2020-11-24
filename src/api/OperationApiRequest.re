include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type injection = {
  operation: Injection.operation,
  password: string,
};

let useCreate = network => {
  let set = (~config, {operation, password}) =>
    (network, config)->OperationsAPI.inject(operation, ~password);
  ApiRequest.useSetter(set, Logs.Operation, ());
};

/* Simulate */

let useSimulate = network => {
  let set = (~config, operation) =>
    (network, config)->OperationsAPI.simulate(operation);
  ApiRequest.useSetter(set, Logs.Operation, ());
};

/* Get list */

let useGet = (~limit=?, ~types=?, ()) => {
  let setOperations = StoreContext.useSetOperations();
  let get = (~config, (network, account: Account.t)) => {
    (network, config)
    ->OperationsAPI.get(account.address, ~limit?, ~types?, ~mempool=true, ())
    ->Future.tapOk(res => setOperations(_ => res));
  };
  ApiRequest.useGetter(get, Logs.Operation);
};
