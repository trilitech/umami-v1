type t('value) =
  | NotAsked
  | Loading
  | Done(Belt.Result.t('value, string));

type operationApiRequest = t(string);

module OperationsAPI = API.Operations(API.TezosClient, API.TezosExplorer);

let useOperation = network => {
  let (request, setRequest) = React.useState(_ => NotAsked);

  let sendRequest = operation => {
    setRequest(_ => Loading);

    network
    ->OperationsAPI.create(operation)
    ->Future.get(result => setRequest(_ => Done(result)));
  };

  (request, sendRequest);
};
