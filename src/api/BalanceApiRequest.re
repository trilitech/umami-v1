module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad =
    (~network, ~requestState as (request, setRequest), ~address: string) => {
  let get = (~config, (network, address)) => {
    (network, config)->BalanceAPI.get(address, ());
  };

  let getRequest =
    ApiRequest.useStoreGetter(~get, ~kind=Logs.Balance, ~setRequest, ());

  React.useEffect3(
    () => {
      if (address != "" && request == ApiRequest.NotAsked) {
        getRequest((network, address));
      };
      None;
    },
    (network, request, address),
  );

  request;
};
