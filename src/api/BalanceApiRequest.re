module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad = (address: string) => {
  let network = StoreContext.useNetwork();

  let (request, setRequest) = StoreContext.useBalanceRequestState(address);

  let get = (~config, (network, address)) => {
    (network, config)->BalanceAPI.get(address, ());
  };

  let getRequest = ApiRequest.useStoreGetter(get, Logs.Balance, setRequest);

  React.useEffect3(
    () => {
      if (address != "" && request == NotAsked) {
        getRequest((network, address));
      };

      None;
    },
    (network, request, address),
  );

  request;
};
