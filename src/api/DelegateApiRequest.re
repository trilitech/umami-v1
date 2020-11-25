include ApiRequest;
module DelegateAPI = API.Delegate(API.TezosClient, API.TezosExplorer);

/* Get delegate */

let useGetDelegate = (account: Account.t) => {
  let network = StoreContext.useNetwork();
  let setAccountDelegate = StoreContext.useSetAccountDelegate();

  ApiRequest.useLoader1(
    (~config, network) =>
      DelegateAPI.getForAccount((network, config), account.address)
      ->Future.tapOk(res => setAccountDelegate(account.address, res)),
    Error.Delegate,
    network,
  );
};

/* Get Bakers */

let useGetBakers = () => {
  let network = StoreContext.useNetwork();

  ApiRequest.useLoader1(
    (~config as _c, network) => DelegateAPI.getBakers(network),
    Error.Delegate,
    network,
  );
};
