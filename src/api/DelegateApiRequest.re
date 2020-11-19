include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient);

/* Get delegate */

let useGetDelegate = account => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config, network) => DelegateAPI.get((network, config), account),
    Error.Delegate,
    network,
  );
};
