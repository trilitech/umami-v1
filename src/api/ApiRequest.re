type t('value) =
  | NotAsked
  | Loading
  | Done(Belt.Result.t('value, string));

/* OPERATION */

type operationApiRequest = t(string);

module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

let useOperation = () => {
  let (network, _) = React.useContext(Network.context);

  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = operation => {
    setRequest(_ => Loading);

    network
    ->OperationsAPI.create(operation)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};

/* BALANCE */

type balanceApiRequest = t(string);

module BalanceAPI = API.Balance(API.TezosClient);

let useBalance = account => {
  let (network, _) = React.useContext(Network.context);

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
