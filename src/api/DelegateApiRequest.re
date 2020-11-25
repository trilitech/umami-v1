include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useGetDelegate = account => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config, network) =>
      DelegateAPI.getForAccount((network, config), account),
    Error.Delegate,
    network,
  );
};
