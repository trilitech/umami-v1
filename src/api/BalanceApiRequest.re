module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad = (address: string) => {
  let network = StoreContext.useNetwork();

  let get = (~config, (network, address)) => {
    (network, config)->BalanceAPI.get(address, ());
  };

  let (getRequest, request) = ApiRequest.useGetter(get, Logs.Balance);

  React.useEffect2(
    () => {
      if (address != "") {
        getRequest((network, address));
      };

      None;
    },
    (network, address),
  );

  request;
};
