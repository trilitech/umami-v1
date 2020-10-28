include ApiRequest;

module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type createOperationApiRequest = t(string);

let useCreateOperation = () => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = operation => {
    setRequest(_ => Loading);

    network
    ->OperationsAPI.create(operation)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* Simulate */

type simulateOperationApiRequest = t(string);

let useSimulateOperation = () => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = operation => {
    setRequest(_ => Loading);

    network
    ->OperationsAPI.simulate(operation)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* Get list */

type getOperationsApiRequest = t(array(Operation.t));

let useGetOperations = (~limit=?, ~types=?, ()) => {
  let network = StoreContext.useNetwork();
  let account = StoreContext.useAccount();

  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect5(
    () => {
      switch (account) {
      | Some(account) =>
        setRequest(_ => Loading);
        network
        ->OperationsAPI.get(account.address, ~limit?, ~types?, ())
        ->Future.get(result => setRequest(_ => Done(result)));
      | None => ()
      };

      None;
    },
    (network, account, limit, types, setRequest),
  );

  request;
};
