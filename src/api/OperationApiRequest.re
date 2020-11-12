include ApiRequest;
module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

/* Create */

type createOperationApiRequest = t(string);

let useCreateOperation = network => {
  let config = ConfigContext.useConfig();

  let (request, setRequest) = React.useState(_ => NotAsked);

  let addError = ErrorsContext.useAddError();

  let sendRequest = operation => {
    setRequest(_ => Loading);

    (network, config)
    ->OperationsAPI.create(operation)
    ->Future.tap(result => {
        switch (result) {
        | Error(msg) =>
          addError(Error.{kind: Operation, msg, timestamp: Js.Date.now()})
        | _ => ()
        };
        setRequest(_ => Done(result));
      });
  };

  (request, sendRequest);
};

/* Simulate */

type simulateOperationApiRequest = t(string);

let useSimulateOperation = network => {
  let config = ConfigContext.useConfig();

  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = operation => {
    setRequest(_ => Loading);

    (network, config)
    ->OperationsAPI.simulate(operation)
    ->Future.get(result => {setRequest(_ => Done(result))});
  };

  (request, sendRequest);
};

/* Get list */

type getOperationsApiRequest = t(array(Operation.t));

let useGetOperations = (~limit=?, ~types=?, ()) => {
  let setOperations = StoreContext.useSetOperations();

  let (request, setRequest) = React.useState(_ => NotAsked);
  let config = ConfigContext.useConfig();

  let get = (network, account: option(Account.t)) => {
    switch (account) {
    | Some(account) =>
      setRequest(_ => Loading);
      (network, config)
      ->OperationsAPI.get(account.address, ~limit?, ~types?, ())
      ->Future.tapOk(res => setOperations(_ => res))
      ->Future.get(result => setRequest(_ => {Done(result)}));
    | None => ()
    };
    ();
  };

  (get, request);
};
