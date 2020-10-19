type t('value) =
  | NotAsked
  | Loading
  | Done(Belt.Result.t('value, string));

/* OPERATION */

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

/* Get list */

type getOperationsApiRequest = t(array(Operation.t));

let useGetOperations = () => {
  let network = StoreContext.useNetwork();
  let account = StoreContext.useAccount();

  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect3(
    () => {
      setRequest(_ => Loading);

      network
      ->OperationsAPI.get(account, ())
      ->Future.get(result => setRequest(_ => Done(result)));
      None;
    },
    (network, account, setRequest),
  );

  request;
};

/* BALANCE */

module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = t(string);

let useBalance = account => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) = React.useState(_ => NotAsked);

  React.useEffect3(
    () => {
      setRequest(_ => Loading);

      network
      ->BalanceAPI.get(account)
      ->Future.get(result => setRequest(_ => Done(result)));

      None;
    },
    (network, account, setRequest),
  );

  request;
};
