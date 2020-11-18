module BalanceAPI = API.Balance(API.TezosClient);

type balanceApiRequest = ApiRequest.t(string);

let useLoad = account => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config, network) => BalanceAPI.get((network, config), account),
    Error.Balance,
    network,
  );
};
